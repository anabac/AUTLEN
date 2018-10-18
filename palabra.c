#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palabra.h"

struct _Palabra {
	char **letras;
	int longitud;
	int simbolo_actual;
};

Palabra *PalabraNuevo(){
	Palabra *p;

	p = (Palabra *) malloc(sizeof(Palabra));
	if (!p) return NULL;

	p->letras = NULL;
	p->longitud = 0;
	p->simbolo_actual = 0;

	return p;
}

void PalabraElimina(Palabra *p){
	int i;

	if (!p) return;

	for (i = 0; i < p->longitud; i++){
		free(p->letras[i]);
	}

	free(p->letras);
	free(p);
}

Palabra *PalabraInsertaLetra(Palabra *p, char *l){
	char *letra;

	if (!p | !l) return NULL;

	letra = (char *) malloc((strlen(l) + 1) * sizeof(char));
	if (!letra) return NULL;

	if (strcpy(letra, l) < 0){
		free(letra);
		return NULL;
	}
	
	// if (!p->letras)
	// 	// p->letras = (char **) malloc(sizeof(char *));
	// 	p->letras = (char **) realloc(NULL, (p->longitud + 1) * sizeof(char *));

	// else
		p->letras = (char **) realloc(p->letras, (p->longitud + 1) * sizeof(char *));

	if (!p->letras){
		free(letra);
		return NULL;
	}

	p->letras[p->longitud] = letra;
	p->longitud++;

	return p;
}

void imprimeCadena(FILE *f, Palabra *p){
	int i;

	if (!p || !f) return;

	fprintf(f, "[(%d)", p->longitud - p->simbolo_actual);
	for (i = p->simbolo_actual; i < p->longitud; i++)
		fprintf(f, " %s", p->letras[i]);
	fprintf(f, "]\n");
}

char *procesarSimbolo(Palabra *p){	
	if (p->longitud <= p->simbolo_actual) 
		return NULL;
	
	return p->letras[p->simbolo_actual++];
}
