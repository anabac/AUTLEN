#ifndef ESTADO_H
#define ESTADO_H

typedef enum TipoEstado {
	NORMAL,
	INICIAL,
	FINAL,
	INICIAL_Y_FINAL
};

typedef struct _Estado Estado;

Estado *EstadoNuevo(char* nombre, TipoEstado tipo);
void EstadoElimina(Estado *e);
void EstadoImprime(Estado *e);
TipoEstado getTipoEstado(Estado *e);

#endif
