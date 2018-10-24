
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "alfabeto.h"
#include "estado.h"
#include "palabra.h"


struct _AFND {
	char *nombre; // nombre del automata

	int max_estados; // numero maximo de estados
	int num_estados;  // numero actual de estados
	Estado **estados;  // coleccion de estados

	Alfabeto *alfabeto;  // alfabeto 

	int num_estados_actuales;  // numero de estados actuales
	Estado **estados_actuales;  // coleccion de estados actuales

	char *simbolo_actual;  // simbolo que se esta procesando
	Palabra *cadena_entrada;  // palabra de entrada

	short ***transiciones;  // matriz cubica para transiciones
};


AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	AFND *afnd;
	int i, j, k;

	afnd = (AFND *) malloc(sizeof(AFND));
	if (!afnd) return NULL;

	// Reservar para el nombre del automata
	afnd->nombre = (char *) malloc((strlen(nombre)+1) * sizeof(char));
	if (!afnd->nombre){
		free(afnd);
		return NULL;
	}

	// Asignacion del nombre del automata
	if (strcpy(afnd->nombre, nombre) < 0){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Creacion del alfabeto del automata con num_simbolos
	afnd->alfabeto = AlfabetoNuevo(num_simbolos);
	if (!afnd->alfabeto){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva de un array de num_estados estados
	afnd->estados = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados){
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva de un array de num_estados para los estados 
	// de cada paso del proceso del automata
	afnd->estados_actuales = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados_actuales){
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva para la cadena de entrada del automata
	afnd->cadena_entrada = PalabraNuevo();
	if (!afnd->cadena_entrada){
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva una matriz cubica de dimensiones
	// num_estados x num_simbolos x num_estados 
	// para guardar las transiciones entre estados del automata

	// Eje estado origen
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
	// Eje simbolo
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
		// Eje estado destino
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
			// Inicializacion a 0
			for (k = 0; k < num_estados; k++)
				afnd->transiciones[i][j][k] = 0;
		}
	}

	// Inicializacion de variables propias del automata
	afnd->max_estados = num_estados;
	afnd->num_estados = 0;
	afnd->num_estados_actuales = 0;
	afnd->simbolo_actual = NULL;

	return afnd;
}

void AFNDElimina(AFND * p_afnd){
	int i, j;

	if (!p_afnd) return;

	free(p_afnd->estados_actuales);
	
	// Eliminacion de todos los estados
	for (i = 0; i < p_afnd->num_estados; i++)
		EstadoElimina(p_afnd->estados[i]);
	free(p_afnd->estados);

	// Eliminacion de la matriz cubica de transiciones
	for (i = 0; i < p_afnd->num_estados; i++){
		for(j = 0; j < AlfabetoGetNumSimbolos(p_afnd->alfabeto); j++)
			free(p_afnd->transiciones[i][j]);
		free(p_afnd->transiciones[i]);
	}
	free(p_afnd->transiciones);

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

	// Tomamos los simbolos del alfabeto del automata
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	
	// Recorremos los estados 
	for (i = 0; i < p_afnd->num_estados; i++){
		// Por cada estado, los simbolos del alfabeto
		for (j = 0; j < num_simbolos; j++){
			fprintf(fd, "\n\tf(%s,%s)={", getNombre(p_afnd->estados[i]),
									  simbolos[j]);
			// Y buscamos si existe una transicion con esas entradas
			for (k = 0; k < p_afnd->num_estados; k++)
				if (p_afnd->transiciones[i][j][k] == 1)
					fprintf(fd, " %s", getNombre(p_afnd->estados[k]));
			
		fprintf(fd, " }");
		}
	}
}

void AFNDImprime(FILE * fd, AFND* p_afnd){
	if (!fd | !p_afnd) return;

	fprintf(fd, "%s={\n\t", p_afnd->nombre);
	
	// Imprimos el nombre del automata
	AlfabetoImprime(fd, p_afnd->alfabeto);
	fprintf(fd, "\n\t");
	
	// La informacion de los estados
	fprintf(fd, "num_estados = %d\n", p_afnd->num_estados);
	fprintf(fd, "\n\t");
	imprimeEstados(fd, p_afnd);
	
	// Y la funcion de transicion
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

	// Tomamos los simbolos del alfabeto del automata
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);

	// Recorremos la lista de estados del automata para encontrar
	// los indices del estado inicial (i) y el final (k)
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

	// Recorremos la lista de simbolos para encontrar el indice 
	// del simbolo en la transicion (j)
	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++){
		if (!strcmp(nombre_simbolo_entrada, simbolos[j]))
			flag = 1;
	}
	if (flag == 0)
		return NULL;

	// Modificamos la matriz de transiciones segun los indices
	p_afnd->transiciones[i][--j][k] = 1;
			
	return NULL;
}

AFND * AFNDInsertaLetra(AFND * p_afnd, char * letra){

	if (!p_afnd || !letra) return NULL;
	// Insertamos la letra en la cadena de entrada del automata
	if (!PalabraInsertaLetra(p_afnd->cadena_entrada, letra)) return NULL;

	return p_afnd;
}

AFND * AFNDInsertaEstado(AFND * p_afnd, char * nombre, int tipo){

	if (!p_afnd || !nombre || tipo < 0 || tipo > 3) return NULL;
	if (p_afnd->max_estados <= p_afnd->num_estados) return NULL;

	// Creacion e insercion del estado en la coleccion de estados
	p_afnd->estados[p_afnd->num_estados] = EstadoNuevo(nombre, tipo);
	if (!p_afnd->estados[p_afnd->num_estados]) return NULL;

	p_afnd->num_estados++;
	return p_afnd;
}

void AFNDImprimeConjuntoEstadosActual(FILE * fd, AFND * p_afnd){
	int i;

	if (!p_afnd || !fd) return;

	// Si no hay estados actuales, la cadena se marca como rechazada
	if (!p_afnd->num_estados_actuales){
		fprintf(fd, "CADENA RECHAZADA\n");
		return;
	}

	fprintf(fd, "ACTUALMENTE EN { ");
	// Por cada estado actual, se imprime este
	for (i = 0; i < p_afnd->num_estados_actuales; i++){
		EstadoImprime(fd, p_afnd->estados_actuales[i]);
		fprintf(fd, " ");
	}
	fprintf(fd, "}\n");
}

void AFNDImprimeCadenaActual(FILE *fd, AFND * p_afnd){

	if (!p_afnd || !fd) return;
	imprimeCadena(fd, p_afnd->cadena_entrada);
}

AFND * AFNDInicializaEstado (AFND * p_afnd){
	int i;

	if (!p_afnd) return NULL;

	p_afnd->num_estados_actuales = 0;
	// Por cada estado, si este es inicial o inicial y final,
	// se añade a la lista de estados actuales
	for (i = 0; i < p_afnd->num_estados; i++){
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL ||
			getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL){
			p_afnd->estados_actuales[p_afnd->num_estados_actuales] = p_afnd->estados[i];
			p_afnd->num_estados_actuales ++;
		}
	}

	return p_afnd;

}

void reiniciaCadena(AFND *p_afnd){

	if (!p_afnd) return;

	// Elimina y vuelve a crear la cadena de entrada del automata
	PalabraElimina(p_afnd->cadena_entrada);
	p_afnd->cadena_entrada = PalabraNuevo();
}

void AFNDProcesaEntrada(FILE * fd, AFND * p_afnd){

	if (!fd || !p_afnd) return;
	
	// Imprime la situacion inicial del automata
	AFNDImprimeConjuntoEstadosActual(fd, p_afnd);
	imprimeCadena(fd, p_afnd->cadena_entrada);
	fprintf(fd, "\n");
	
	// Procesa el primer simbolo de la cadena de entrada
	p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	
	// Mientras se pueda seguir procesando
	while (p_afnd->simbolo_actual) {
		AFNDTransita(p_afnd);

		AFNDImprimeConjuntoEstadosActual(fd, p_afnd);
		
		// Si no existen estados actuales, se acaba el proceso
		if (p_afnd->num_estados_actuales == 0){
			reiniciaCadena(p_afnd);
			return;
		} 
		
		imprimeCadena(fd, p_afnd->cadena_entrada);
		fprintf(fd, "\n");

		p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	}

	reiniciaCadena(p_afnd);

}
void AFNDTransita(AFND * p_afnd){
	int i, j, k, m, flag;
	int num_simbolos;
	char **simbolos;
	int num_nuevos_estados;
	Estado **nuevos_estados;
	short *transiciones;

	if (!p_afnd) return;

	// Reserva para la nueva coleccion de estados actuales
	nuevos_estados = (Estado **) malloc(p_afnd->num_estados * sizeof(Estado *));
	if (!nuevos_estados) return;
	num_nuevos_estados = 0;

	// Toma los simbolos del alfabeto del automata
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	
	// Recorre los simbolos para encontrar el indice (j) del simbolo actual
	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++)
		if (!strcmp(p_afnd->simbolo_actual, simbolos[j]))
			flag = 1;
	j--;

	// Recorre los estados actuales del automata 
	for (k = 0; k < p_afnd->num_estados_actuales; k++){
		// Encuentra, por cada estado actual, su indice dentro de los estados (i)
		for (i = 0, flag = 0; i < p_afnd->num_estados && flag == 0; i++)
			if (!strcmp(getNombre(p_afnd->estados[i]), getNombre(p_afnd->estados_actuales[k])))
				flag = 1;
		i--;
		
		// Toma las transiciones desde el estado con el simbolo actual
		transiciones = p_afnd->transiciones[i][j];
		// Si existe alguna transicion, añade el nuevo estado a la lista de 
		// nuevos estados actuales
		for (m = 0;  m < p_afnd->num_estados; m++)
			if (transiciones[m]){
				nuevos_estados[num_nuevos_estados] = p_afnd->estados[m];
				num_nuevos_estados ++;
			}
	}

	// Asignacion de los nuevos valores
	free(p_afnd->estados_actuales);
	p_afnd->estados_actuales = nuevos_estados;
	p_afnd->num_estados_actuales = num_nuevos_estados;
}
