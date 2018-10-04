#ifndef PALABRA_H
#define PALABRA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Palabra Palabra;

Palabra *insertaLetra(Palabra *w, Simbolo *s);
int imprimeCadena(Palabra *w);


#endif
