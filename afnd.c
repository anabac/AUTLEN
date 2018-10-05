
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "alfabeto.h"

struct _AFND {
	char *nombre;
	int num_estados;
	ABETO *alfabeto;
} AFND;


AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	if (!nombre || !num_estados || !num_simbolos) return NULL;

	AFND *afnd = (AFND *) malloc(sizeof(AFND));
	if (!afnd) return NULL;

	afnd->nombre = (char *) malloc(sizeof(char)*strlen(nombre));
	if (!afnd->nombre) return NULL;
	strcpy(afnd->nombre, nombre);

	afnd->num_estados = num_estados;
	afnd->alfabeto = ABETONuevo(num_simbolos);
	if (!afnd->alfabeto) return NULL;

	return afnd;
}

// void AFNDImprime(FILE * fd, AFND* p_afnd){
	
// }
