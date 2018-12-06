
#ifndef ALFABETO_H
#define ALFABETO_H

typedef struct _Alfabeto Alfabeto;

Alfabeto *AlfabetoNuevo(int num_simbolos);
void AlfabetoInsertaSimbolo(Alfabeto *alf, char *simbolo);
void AlfabetoImprime(FILE *f, Alfabeto *alf);
void AlfabetoElimina(Alfabeto *alf);
char **AlfabetoGetSimbolos(Alfabeto *alf);
int AlfabetoGetNumSimbolos(Alfabeto *alf);

int getIndiceSimbolo(Alfabeto *alf, char *simbolo);

#endif 
