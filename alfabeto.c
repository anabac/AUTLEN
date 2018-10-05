#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alfabeto.h"

struct _Alfabeto {
	int num_simbolos;
	int num_simbolos_insertados;
	char **simbolos;
};


Alfabeto *AlfabetoNuevo(int num_simbolos){
	if (!num_simbolos) return NULL;
	
	Alfabeto *alf = (Alfabeto *) malloc(sizeof(Alfabeto));
	if (!alf) return NULL;

	alf->num_simbolos = num_simbolos;
	alf->num_simbolos_insertados = 0;
	alf->simbolos = (char **) malloc(sizeof(char *)*num_simbolos);
	if (!alf->simbolos) return NULL;

	return alf;
}

void AlfabetoInsertaSimbolo(Alfabeto *alf, char *simbolo){
	int i;
	
	if (!alf) return;
	if (alf->num_simbolos <= alf->num_simbolos_insertados) return;

	for (i = 0; i < alf->num_simbolos_insertados; i++)
		if (!strcmp(alf->simbolos[i], simbolo)) return;

	alf->simbolos[alf->num_simbolos_insertados] = (char *) malloc((strlen(simbolo)+1)*sizeof(char));
	if (!alf->simbolos[alf->num_simbolos_insertados]) return;
	
	strcpy(alf->simbolos[alf->num_simbolos_insertados], simbolo);
	alf->num_simbolos_insertados ++;
}

void AlfabetoImprime(FILE *f, Alfabeto *alf){
	int i;

	if (!f || !alf) return;

	fprintf(f, "\n\tnum_simbolos = %d\n\n", alf->num_simbolos);
	
	fprintf(f, "\tA={ ");
	for (i = 0; i < alf->num_simbolos_insertados; i++)
		fprintf(f, "%s ", alf->simbolos[i]);
	fprintf(f, "}\n");
}

void AlfabetoElimina(Alfabeto *alf){
	int i;

	if (!alf) return;

	for (i = 0; i < alf->num_simbolos_insertados; i++)
		free(alf->simbolos[i]);

	free(alf->simbolos);
	free(alf);	
}

char **AlfabetoGetSimbolos(Alfabeto *alf){
	if (!alf) return NULL;
	return alf->simbolos;
}

int AlfabetoGetNumSimbolos(Alfabeto *alf){
	if (!alf) return 0;
	return alf->num_simbolos;
}
