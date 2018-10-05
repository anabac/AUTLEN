#ifndef ESTADO_H
#define ESTADO_H

typedef enum _TipoEstado {
	NORMAL,
	INICIAL,
	FINAL,
	INICIAL_Y_FINAL
} TipoEstado;

typedef struct _Estado Estado;

Estado *EstadoNuevo(char* nombre, TipoEstado tipo);
void EstadoElimina(Estado *e);
void EstadoImprime(FILE *fd, Estado *e);
TipoEstado getTipoEstado(Estado *e);
char * getNombre(Estado *e);

#endif
