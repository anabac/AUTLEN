typedef enum TipoEstado {
  INICIAL,
  NORMAL,
  FINAL
};

typedef struct _Estado Estado;

Estado *EstadoCrea(TipoEstado tipo, char* nombre);
void EstadoElimina(Estado *e);
