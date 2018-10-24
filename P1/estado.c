#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estado.h"

struct _Estado {
	char *nombre; // nombre del estado
	TipoEstado tipo; // tipo del estado
};

Estado *EstadoNuevo(char* nombre, TipoEstado tipo){
	Estado *e;

	//Reservar memoria para el estado
	e = (Estado *) malloc(sizeof(Estado));
	if (!e) return NULL;

	//Reservar memoria para el nombre, se introduce una copia
	e->nombre = (char *) malloc(sizeof(char) * (strlen(nombre)+1));
	if (!e->nombre){
		free(e);
		return NULL;
	}
	//Copiar el nombre
	if (strcpy(e->nombre, nombre) < 0){
	free(e->nombre);
	free(e);
	return NULL;
	}
	//Asignar el tipo
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

	//"Vacía" inicial si no es inicial
	// y final si no es final
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
	//Imprime "->" antes del nombre de los iniciales
	//y "*" después de los finales
	fprintf(fd, "%s%s%s", inicial, e->nombre, final);
}

//Getters

TipoEstado getTipoEstado(Estado *e){
	if (!e) return NORMAL;
	return e->tipo;
}

char * getNombre(Estado *e){
	if (!e) return NULL;
	return e->nombre;
}
