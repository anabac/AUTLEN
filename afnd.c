#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "alfabeto.h"
#include "estado.h"
#include "palabra.h"


// Vale, lo de las transiciones lo he hecho con una matriz porque
// es lo que le entendi al tipo que tenia que hacer, pero el problema es
// que claro, se me sobrescriben los valores y no me apetece pensar... asi que 
// funcionar no funciona :)

struct _AFND {
	char *nombre;

	int max_estados;
	int num_estados;
	Estado **estados;

	Alfabeto *alfabeto;

	int num_estados_actuales;
	Estado **estados_actuales;

	int simbolo_actual;
	Palabra *cadena_entrada;

	char ***transiciones;
};


AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	AFND *afnd;
	int i;

	afnd = (AFND *) malloc(sizeof(AFND));
	if (!afnd) return NULL;

	afnd->nombre = (char *) malloc((strlen(nombre)+1) * sizeof(char));
	if (!afnd->nombre){
		free(afnd);
		return NULL;
	}

	if (strcpy(afnd->nombre, nombre) < 0){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->alfabeto = AlfabetoNuevo(num_simbolos);
	if (!afnd->alfabeto){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->estados = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados){
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->estados_actuales = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados_actuales){
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->cadena_entrada = PalabraNuevo();
	if (!afnd->cadena_entrada){
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->transiciones = (char ***) malloc(sizeof(char **)*num_estados);
	if (!afnd->transiciones){
		free(afnd->cadena_entrada);
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}
	for (i = 0; i < num_estados; i++){
		afnd->transiciones[i] = (char **) malloc(sizeof(char *)*num_estados);
		if (!afnd->transiciones[i]){
			for (i-=1 ; i >= 0; i--)
				free(afnd->transiciones[i]);
			free(afnd->transiciones);
			free(afnd->cadena_entrada);
			free(afnd->estados_actuales);
			free(afnd->estados);
			AlfabetoElimina(afnd->alfabeto);
			free(afnd->nombre);
			free(afnd);
			return NULL;
		}
	}

	afnd->max_estados = num_estados;
	afnd->num_estados = 0;
	afnd->num_estados_actuales = 0;
	afnd->simbolo_actual = 0;

	return afnd;
}

void AFNDElimina(AFND * p_afnd){
	int i;

	if (!p_afnd) return;

	for (i = 0; i < p_afnd->num_estados_actuales; i++)
		EstadoElimina(p_afnd->estados_actuales[i]);
	free(p_afnd->estados_actuales);
	
	for (i = 0; i < p_afnd->num_estados; i++)
		EstadoElimina(p_afnd->estados[i]);
	free(p_afnd->estados);

	AlfabetoElimina(p_afnd->alfabeto);
	free(p_afnd->nombre);
	free(p_afnd);
}

void imprimeEstados(FILE *fd, AFND *p_afnd){
	int i;

	fprintf(fd, "Q={");
	for (i = 0; i < p_afnd->num_estados; i++){
		EstadoImprime(fd, p_afnd->estados[i]);
		fprintf(fd, " ");
	}
	fprintf(fd, "}\n");
}

void imprimeFuncionesTransicion(FILE *fd, AFND *p_afnd){
	int i, j, k, num_simbolos;
	char **simbolos;

	if (!fd || !p_afnd) return;

	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	
	for (i = 0; i < p_afnd->num_estados; i++)
		for (j = 0; j < num_simbolos; j++){
			fprintf(fd, "\n\tf(%s,%s)={", getNombre(p_afnd->estados[i]),
									  simbolos[j]);
			for (k = 0; k < p_afnd->num_estados; k++)
				if (p_afnd->transiciones[i][k])
					if (!strcmp(p_afnd->transiciones[i][k], simbolos[j]))
						fprintf(fd, " %s", getNombre(p_afnd->estados[k]));
			
		fprintf(fd, " }");
		}
}

void AFNDImprime(FILE * fd, AFND* p_afnd){
	if (!fd | !p_afnd) return;

	fprintf(fd, "%s={\n\t", p_afnd->nombre);
	AlfabetoImprime(fd, p_afnd->alfabeto);
	fprintf(fd, "\n\t");
	fprintf(fd, "num_estados = %d\n", p_afnd->num_estados);
	fprintf(fd, "\n\t");
	imprimeEstados(fd, p_afnd);
	fprintf(fd, "\nFunción de Transición = {\n");
	imprimeFuncionesTransicion(fd, p_afnd);
	fprintf(fd, "\n  }\n");
	fprintf(fd, "}\n");
}

AFND * AFNDInsertaSimbolo(AFND * p_afnd, char * simbolo){

	if (!p_afnd || !simbolo) return NULL;

	AlfabetoInsertaSimbolo(p_afnd->alfabeto, simbolo);
	return p_afnd;
}

AFND * AFNDInsertaTransicion(AFND * p_afnd, 
                             char * nombre_estado_i, 
                             char * nombre_simbolo_entrada, 
                             char * nombre_estado_f ){

	int i, j;

	if (!p_afnd || !nombre_estado_i || ! nombre_simbolo_entrada || !nombre_estado_f)
		return NULL;

	for (i = 0; i < p_afnd->num_estados; i++)
		if (!strcmp(nombre_estado_i, getNombre(p_afnd->estados[i])))
			for (j = 0; j < p_afnd->num_estados; j++)
				if (!strcmp(nombre_estado_f, getNombre(p_afnd->estados[j]))){
					p_afnd->transiciones[i][j] = (char *) malloc(sizeof(char)*(strlen(nombre_simbolo_entrada)+1));
					if (!p_afnd->transiciones[i][j]) return NULL;
					
					if (strcpy(p_afnd->transiciones[i][j], nombre_simbolo_entrada) < 0) return NULL;
					return p_afnd;
				}

	return NULL;
}

AFND * AFNDInsertaLetra(AFND * p_afnd, char * letra){

	if (!p_afnd || !letra) return NULL;
	if (!PalabraInsertaLetra(p_afnd->cadena_entrada, letra)) return NULL;

	return p_afnd;
}

AFND * AFNDInsertaEstado(AFND * p_afnd, char * nombre, int tipo){

	if (!p_afnd || !nombre || tipo < 0 || tipo > 3) return NULL;
	if (p_afnd->max_estados <= p_afnd->num_estados) return NULL;

	p_afnd->estados[p_afnd->num_estados] = EstadoNuevo(nombre, tipo);
	if (!p_afnd->estados[p_afnd->num_estados]) return NULL;

	p_afnd->num_estados++;
	return p_afnd;
}

void AFNDImprimeConjuntoEstadosActual(FILE * fd, AFND * p_afnd){
	int i;

	if (!p_afnd || !fd) return;
	if (!p_afnd->num_estados_actuales) return;

	fprintf(fd, "ACTUALMENTE EN {");
	for (i = 0; i < p_afnd->num_estados_actuales; i++)
		EstadoImprime(fd, p_afnd->estados_actuales[i]);
	fprintf(fd, " }\n");
}

void AFNDImprimeCadenaActual(FILE *fd, AFND * p_afnd){

	if (!p_afnd || !fd) return;
	imprimeCadena(fd, p_afnd->cadena_entrada);
}

AFND * AFNDInicializaEstado (AFND * p_afnd){
	int i;

	if (!p_afnd) return NULL;

	for (i = 0; i < p_afnd->num_estados; i++)
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL ||
			getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL)
			p_afnd->estados_actuales[p_afnd->num_estados_actuales] = p_afnd->estados[i];

	return p_afnd;

}

// TODO: Implementar estas funciones

void AFNDProcesaEntrada(FILE * fd, AFND * p_afnd){

}
void AFNDTransita(AFND * p_afnd){

}
