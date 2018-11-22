
#include "afnd1O.h"
#include <string.h>


AFND *AFND1ODeSimbolo(char *simbolo){

	AFND *afnd1O;
	char nombre[60] = "afnd1O_";

	if (!simbolo) return NULL;

	afnd1O = AFNDNuevo(strcat(nombre, simbolo), 2, 1);
	if (!afnd1O) return NULL;

	AFNDInsertaSimbolo(afnd1O, simbolo);

	AFNDInsertaEstado(afnd1O, "q0", INICIAL);
	AFNDInsertaEstado(afnd1O, "qf", FINAL);

	AFNDInsertaTransicion(afnd1O, "q0", simbolo, "qf");

	return afnd1O;
}

AFND *AFND1ODeLambda(){
	
	AFND *afnd1O;

	afnd1O = AFNDNuevo("afnd1O_lambda", 2, 0);
	if (!afnd1O) return NULL;

	AFNDInsertaEstado(afnd1O, "q0", INICIAL);
	AFNDInsertaEstado(afnd1O, "qf", FINAL);

	AFNDInsertaLTransicion(afnd1O, "q0", "qf");

	return afnd1O;
}

AFND *AFND1ODeVacio(){

	AFND *afnd1O;

	afnd1O = AFNDNuevo("afnd1O_vacio", 2, 0);
	if (!afnd1O) return NULL;

	AFNDInsertaEstado(afnd1O, "q0", INICIAL);
	AFNDInsertaEstado(afnd1O, "qf", FINAL);

	return afnd1O;
}

AFND *AFNDAAFND1O(AFND *p_afnd){

	if (!p_afnd) return NULL;
}