#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estado.h"

struct _Estado {
	char *nombre;
	TipoEstado tipo;
};

Estado *EstadoNuevo(char* nombre, TipoEstado tipo){
	Estado *e;

	e = (Estado *) malloc(sizeof(Estado));
	if (!e) return NULL;

	e->nombre = (char *) malloc(sizeof(char) * strlen(nombre));
	if (!e->nombre){
		free(e);
		return NULL;
	}
	if (strcpy(e->nombre, nombre) < 0){
	free(e->nombre);
	free(e);
	return NULL;
	}
	e->tipo = tipo;

	return e;
}

void EstadoElimina(Estado *e){
	if(!e) return;

	free(e->nombre);
	free(e);
}

void EstadoImprime(FILE *fd, Estado *e){
	char inicial[] = "->";
	char final[] = "*";

	if (!fd | !e) return;

	switch(e->tipo){
		case INICIAL:
			final[0] = '\0';
			break;
		case FINAL:
			inicial[0] = '\0';
			break;
		case INICIAL_Y_FINAL:
			break;
		default:
			final[0] = '\0';
			inicial[0] = '\0';
			break;
	}

	fprintf(fd, "%s%s%s", inicial, e->nombre, final);
}

TipoEstado getTipoEstado(Estado *e){
	if (!e) return NORMAL;
	return e->tipo;
}
