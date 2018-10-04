#ifndef PALABRA_H
#define PALABRA_H

typedef struct _Palabra Palabra;

Palabra *PalabraNuevo();
void PalabraElimina(Palabra *p);
Palabra *PalabraInsertaLetra(Palabra *p, char *l);
int imprimeCadena(Palabra *p);


#endif
