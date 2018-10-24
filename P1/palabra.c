#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palabra.h"

struct _Palabra {
	char **letras; // colección de símbolos (char *)
	int longitud; // número de símbolos de la palabra
	int simbolo_actual; // índice del siguiente símbolo a procesar
};

Palabra *PalabraNuevo(){
	Palabra *p;

	// Reservar memoria para la palabra
	p = (Palabra *) malloc(sizeof(Palabra));
	if (!p) return NULL;

	// Inicializar variables
	p->letras = NULL;
	p->longitud = 0;
	p->simbolo_actual = 0;

	return p;
}

void PalabraElimina(Palabra *p){
	int i;

	if (!p) return;

	// Liberar símbolos
	for (i = 0; i < p->longitud; i++){
		free(p->letras[i]);
	}

	free(p->letras);
	free(p);
}

Palabra *PalabraInsertaLetra(Palabra *p, char *l){
	char *letra;

	if (!p | !l) return NULL;

	// Reservar memoria para el símbolo
	letra = (char *) malloc((strlen(l) + 1) * sizeof(char));
	if (!letra) return NULL;

	// Copiar el símbolo
	if (strcpy(letra, l) < 0){
		free(letra);
		return NULL;
	}
	
	// if (!p->letras)
	// 	// p->letras = (char **) malloc(sizeof(char *));
	// 	p->letras = (char **) realloc(NULL, (p->longitud + 1) * sizeof(char *));

	// else

	//Ampliar el array de símbolos en 1 para insertar el nuevo
	p->letras = (char **) realloc(p->letras, (p->longitud + 1) * sizeof(char *));
	if (!p->letras){
		free(letra);
		return NULL;
	}

	//Insertar el símbolo e incrementar la longitud de la palabra
	p->letras[p->longitud] = letra;
	p->longitud++;

	return p;
}

void imprimeCadena(FILE *f, Palabra *p){
	int i;

	if (!p || !f) return;

	//Imprimir el número de símbolos por procesar
	fprintf(f, "[(%d)", p->longitud - p->simbolo_actual);
	//Imprimir los símbolos por procesar
	for (i = p->simbolo_actual; i < p->longitud; i++)
		fprintf(f, " %s", p->letras[i]);
	fprintf(f, "]\n");
}

char *procesarSimbolo(Palabra *p){	
	if (p->longitud <= p->simbolo_actual) 
		return NULL;
	
	//Devolver el símbolo que toca y aumentar el índice al siguiente
	return p->letras[p->simbolo_actual++];
}
