#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estado.h"

struct _Estado {
  TipoEstado tipo;
  char *nombre;
};

Estado *EstadoCrea(TipoEstado tipo, char* nombre){
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
