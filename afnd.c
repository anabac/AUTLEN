#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "alfabeto.h"
#include "estado.h"
#include "palabra.h"


struct _AFND {
	char *nombre;

	int max_estados;
	int num_estados;
	Estado **estados;

	Alfabeto *alfabeto;

	int num_estados_actuales;
	Estado **estados_actuales;

	char *simbolo_actual;
	Palabra *cadena_entrada;

	short ***transiciones;
};


AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	AFND *afnd;
	int i, j, k;

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

	afnd->transiciones = (short ***) malloc(sizeof(short **)*num_estados);
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
		afnd->transiciones[i] = (short **) malloc(sizeof(short *)*num_simbolos);
		if (!afnd->transiciones[i]){
			for (i --; i >= 0; i--)
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
		for (j = 0; j < num_simbolos; j++){
			afnd->transiciones[i][j] = (short *) malloc(sizeof(short)*num_estados);
			if (!afnd->transiciones[i][j]){
				for (j --; j >= 0; j--)
					free(afnd->transiciones[i][j]);
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
			for (k = 0; k < num_estados; k++)
				afnd->transiciones[i][j][k] = 0;
		}
	}

	afnd->max_estados = num_estados;
	afnd->num_estados = 0;
	afnd->num_estados_actuales = 0;
	afnd->simbolo_actual = NULL;

	return afnd;
}

void AFNDElimina(AFND * p_afnd){
	int i;

	if (!p_afnd) return;

	free(p_afnd->estados_actuales);
	
	for (i = 0; i < p_afnd->num_estados; i++)
		EstadoElimina(p_afnd->estados[i]);
	free(p_afnd->estados);

	AlfabetoElimina(p_afnd->alfabeto);
	PalabraElimina(p_afnd->cadena_entrada);
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
				if (p_afnd->transiciones[i][j][k] == 1)
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

	int i, j, k, flag, num_simbolos;
	char **simbolos;

	if (!p_afnd || !nombre_estado_i || ! nombre_simbolo_entrada || !nombre_estado_f)
		return NULL;

	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);

	flag = 0;
	for (j = 0, flag = 0; j < p_afnd->num_estados && flag < 2; j++){
		if (!strcmp(nombre_estado_i, getNombre(p_afnd->estados[j]))){
			i = j;
			flag ++;
		}
		if (!strcmp(nombre_estado_f, getNombre(p_afnd->estados[j]))){
			k = j;
			flag ++;
		}
	}
	if (flag < 2)
		return NULL;

	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++){
		if (!strcmp(nombre_simbolo_entrada, simbolos[j]))
			flag = 1;
	}
	if (flag == 0)
		return NULL;

	p_afnd->transiciones[i][--j][k] = 1;
			
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

	for (i = 0; i < p_afnd->num_estados; i++){
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL ||
			getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL){
			p_afnd->estados_actuales[p_afnd->num_estados_actuales] = p_afnd->estados[i];
			p_afnd->num_estados_actuales++;
		}
	}

	return p_afnd;

}

// TODO: Implementar estas funciones

void AFNDProcesaEntrada(FILE * fd, AFND * p_afnd){
	char * simbolo_actual;
	int i;

	if (!fd || !p_afnd) return;

	p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	while (p_afnd->simbolo_actual) {
		AFNDTransita(p_afnd);

		AFNDImprimeConjuntoEstadosActual(fd, p_afnd);
		imprimeCadena(fd, p_afnd->cadena_entrada);
		fprintf(fd, "\n");

		free(p_afnd->simbolo_actual);
		p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	}

}
void AFNDTransita(AFND * p_afnd){
	int i, j, k, m, flag;
	int num_simbolos;
	char **simbolos;
	int num_nuevos_estados;
	Estado **nuevos_estados;
	short *transiciones;

	if (!p_afnd) return;

	nuevos_estados = (Estado **) malloc(p_afnd->num_estados * sizeof(Estado *));
	if (!nuevos_estados) return;
	num_nuevos_estados = 0;

	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++)
		if (!strcmp(p_afnd->simbolo_actual, simbolos[j]))
			flag = 1;
	j--;

	for (k = 0; k < p_afnd->num_estados_actuales; k++){
		for (i = 0, flag = 0; i < p_afnd->num_estados && flag == 0; i++)
			if (!strcmp(getNombre(p_afnd->estados[i]), getNombre(p_afnd->estados_actuales[k])))
				flag = 1;
		i--;
		
		transiciones = p_afnd->transiciones[i][j];
		for (m = 0;  m < p_afnd->num_estados; m++)
			if (transiciones[m]){
				nuevos_estados[num_nuevos_estados] = p_afnd->estados[m];
				num_nuevos_estados ++;
			}
	}

	free(p_afnd->estados_actuales);
	p_afnd->estados_actuales = nuevos_estados;
	p_afnd->num_estados_actuales = num_nuevos_estados;
}
