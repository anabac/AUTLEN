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
	Estado *estados;

	int max_simbolos;
	int num_simbolos;
	Alfabeto alfabeto;

	int num_estados_actuales;
	Estado *estados_actuales;

	int simbolo_actual;
	Palabra cadena_entrada;
};

AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	AFND *afnd;
	int i;

	afnd = (AFND *) malloc(sizeof(AFND));
	if (!afnd) return NULL;

	afnd->nombre = (char *) malloc(strlen(nombre) * sizeof(char));
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

	afnd->estados = (Estado *) malloc(num_estados * sizeof(Estado));
	if (!afnd->estados){
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->estados_actuales = (Estado *) malloc(num_estados * sizeof(Estado));
	if (!afnd->estados_actuales){
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->cadena_entrada = PalabraNuevo();
	if (!afnd->cadena_entrada){
		free(estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	afnd->max_estados = num_estados;
	afnd->num_estados = 0;
	afnd->max_simbolos = num_simbolos;
	afnd->num_simbolos = 0;
	afnd->num_estados_actuales = 0;
	afnd->simbolo_actual = 0;

	return afnd;
}

void AFNDElimina(AFND * p_afnd){
	if (!p_afnd) return;
	free(estados_actuales);
	free(afnd->estados);
	AlfabetoElimina(afnd->alfabeto);
	free(afnd->nombre);
	free(afnd);
}

void imprimeEstados(FILE *fd, AFND *p_afnd){
	int i;

	fprintf(fd, "Q={");
	for (i = 0; i < p_afnd->num_estados; i++){
		EstadoImprime(p_afnd->estados[i]);
		fprintf(fd, " ");
	}
	fprintf(fd, "}\n");
}

void imprimeFuncionesTransicion(FILE *fd, AFND *p_afnd){
	// TODO: Imprimir funciones de transicion
}

void AFNDImprime(FILE * fd, AFND* p_afnd){
	if (!fd | !p_afnd) return;

	fprintf(fd, "%s={\n\t", p_afnd->nombre);
	fprintf(fd, "num_simbolos = %d\n", p_afnd->num_simbolos);
	fprintf(fd, "\n\t");
	AlfabetoImprime(fd, p_afnd->alfabeto);
	fprintf(fd, "\n\t");
	fprintf(fd, "num_estados = %d\n", p_afnd->num_estados);
	fprintf(fd, "\n\t");
	imprimeEstados(fd, p_afnd);
	fprintf(fd, "\n\t");
	imprimeFuncionesTransicion(fd, p_afnd);
	fprintf(fd, "}\n");
}

// TODO: Implementar estas funciones
AFND * AFNDInsertaSimbolo(AFND * p_afnd, char * simbolo);
AFND * AFNDInsertaEstado(AFND * p_afnd, char * nombre, int tipo);
AFND * AFNDInsertaTransicion(AFND * p_afnd, 
                             char * nombre_estado_i, 
                             char * nombre_simbolo_entrada, 
                             char * nombre_estado_f );
AFND * AFNDInsertaLetra(AFND * p_afnd, char * letra);
void AFNDImprimeConjuntoEstadosActual(FILE * fd, AFND * p_afnd);
void AFNDImprimeCadenaActual(FILE *fd, AFND * p_afnd);
AFND * AFNDInicializaEstado (AFND * p_afnd);
void AFNDProcesaEntrada(FILE * fd, AFND * p_afnd);
void AFNDTransita(AFND * p_afnd);
