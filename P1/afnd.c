
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "alfabeto.h"
#include "estado.h"
#include "palabra.h"


struct _AFND {
	char *nombre; // nombre del automata

	int max_estados; // numero maximo de estados
	int num_estados;  // numero actual de estados
	Estado **estados;  // coleccion de estados

	Alfabeto *alfabeto;  // alfabeto 

	int num_estados_actuales;  // numero de estados actuales
	Estado **estados_actuales;  // coleccion de estados actuales

	char *simbolo_actual;  // simbolo que se esta procesando
	Palabra *cadena_entrada;  // palabra de entrada

	short ***transiciones;  // matriz cubica para transiciones
	short **lambdas;
};

int recursividad_maxima(AFND *p_afnd, Estado **sin_procesar, Estado **procesados, int num_procesados, int num_sin_procesar, int proc);
void imprimeLTransiciones(FILE *fd, AFND *p_afnd);

int indiceDeEstado(AFND *p_afnd, char * nombre){
	int i, flag = 0;

	for (i = 0, flag = 0; i < p_afnd->num_estados && flag == 0; i++)
		if (!strcmp(nombre, getNombre(p_afnd->estados[i])))
			return i;

	return -1;

}

AFND * AFNDNuevo(char * nombre, int num_estados, int num_simbolos){
	AFND *afnd;
	int i, j, k;

	afnd = (AFND *) malloc(sizeof(AFND));
	if (!afnd) return NULL;

	// Reservar para el nombre del automata
	afnd->nombre = (char *) malloc((strlen(nombre)+1) * sizeof(char));
	if (!afnd->nombre){
		free(afnd);
		return NULL;
	}

	// Asignacion del nombre del automata
	if (strcpy(afnd->nombre, nombre) < 0){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Creacion del alfabeto del automata con num_simbolos
	afnd->alfabeto = AlfabetoNuevo(num_simbolos);
	if (!afnd->alfabeto){
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva de un array de num_estados estados
	afnd->estados = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados){
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva de un array de num_estados para los estados 
	// de cada paso del proceso del automata
	afnd->estados_actuales = (Estado **) malloc(num_estados * sizeof(Estado *));
	if (!afnd->estados_actuales){
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva para la cadena de entrada del automata
	afnd->cadena_entrada = PalabraNuevo();
	if (!afnd->cadena_entrada){
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}

	// Reserva una matriz cubica de dimensiones
	// num_estados x num_simbolos x num_estados 
	// para guardar las transiciones entre estados del automata

	// Eje estado origen
	afnd->transiciones = (short ***) malloc(sizeof(short **)*num_estados);
	if (!afnd->transiciones){
		free(afnd->cadena_entrada);
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}
	// Eje simbolo
	for (i = 0; i < num_estados; i++){
		afnd->transiciones[i] = (short **) malloc(sizeof(short *)*(num_simbolos));
		if (!afnd->transiciones[i]){
			for (i --; i >= 0; i--)
				free(afnd->transiciones[i]);
			free(afnd->transiciones);
			free(afnd->cadena_entrada);
			free(afnd->estados_actuales);
			free(afnd->estados);
			AlfabetoElimina(afnd->alfabeto);
			free(afnd->nombre);
			free(afnd);
			return NULL;
		}
		// Eje estado destino
		for (j = 0; j < num_simbolos; j++){
			afnd->transiciones[i][j] = (short *) malloc(sizeof(short)*num_estados);
			if (!afnd->transiciones[i][j]){
				for (j --; j >= 0; j--)
					free(afnd->transiciones[i][j]);
				free(afnd->transiciones[i]);
				free(afnd->transiciones);
				free(afnd->cadena_entrada);
				free(afnd->estados_actuales);
				free(afnd->estados);
				AlfabetoElimina(afnd->alfabeto);
				free(afnd->nombre);
				free(afnd);
				return NULL;
			}
			// Inicializacion a 0
			for (k = 0; k < num_estados; k++)
				afnd->transiciones[i][j][k] = 0;
		}
	}

	afnd->lambdas = (short **) malloc(sizeof(short *) * num_estados);
	if (!afnd->lambdas){
		for (i = 0; i < num_estados; i++){
			for(j = 0; j < num_simbolos; j++)
				free(afnd->transiciones[i][j]);
			free(afnd->transiciones[i]);
		}
		free(afnd->transiciones);
		free(afnd->cadena_entrada);
		free(afnd->estados_actuales);
		free(afnd->estados);
		AlfabetoElimina(afnd->alfabeto);
		free(afnd->nombre);
		free(afnd);
		return NULL;
	}
	for (i = 0; i < num_estados; i++){
		afnd->lambdas[i] = (short *) malloc(sizeof(short) * num_estados);
		if (!afnd->lambdas[i]){
			for (i--; i >= 0; i--)
				free(afnd->lambdas[i]);
			free(afnd->lambdas);
			for (i = 0; i < num_estados; i++){
				for(j = 0; j < num_simbolos; j++)
					free(afnd->transiciones[i][j]);
				free(afnd->transiciones[i]);
			}
			free(afnd->transiciones);
			free(afnd->cadena_entrada);
			free(afnd->estados_actuales);
			free(afnd->estados);
			AlfabetoElimina(afnd->alfabeto);
			free(afnd->nombre);
			free(afnd);
			return NULL;
		}

		for (k = 0; k < num_estados; k++){
			if (i == k)
				afnd->lambdas[i][k] = 1;
			else
				afnd->lambdas[i][k] = 0;
		}
	}

	// Inicializacion de variables propias del automata
	afnd->max_estados = num_estados;
	afnd->num_estados = 0;
	afnd->num_estados_actuales = 0;
	afnd->simbolo_actual = NULL;

	return afnd;
}

void AFNDElimina(AFND * p_afnd){
	int i, j;

	if (!p_afnd) return;

	free(p_afnd->estados_actuales);
	
	// Eliminacion de todos los estados
	for (i = 0; i < p_afnd->num_estados; i++)
		EstadoElimina(p_afnd->estados[i]);
	free(p_afnd->estados);

	// Eliminacion de la matriz cubica de transiciones
	for (i = 0; i < p_afnd->num_estados; i++){
		for(j = 0; j < AlfabetoGetNumSimbolos(p_afnd->alfabeto); j++)
			free(p_afnd->transiciones[i][j]);
		free(p_afnd->transiciones[i]);
	}
	free(p_afnd->transiciones);

	// Eliminación de la matriz de transiciones lambda
	for (i = 0; i < p_afnd->num_estados; i++)
		free(p_afnd->lambdas[i]);
	free(p_afnd->lambdas);

	AlfabetoElimina(p_afnd->alfabeto);
	PalabraElimina(p_afnd->cadena_entrada);
	free(p_afnd->nombre);
	free(p_afnd);
}

void imprimeEstados(FILE *fd, AFND *p_afnd){
	int i;

	fprintf(fd, "Q={");
	for (i = 0; i < p_afnd->num_estados; i++){
		EstadoImprime(fd, p_afnd->estados[i]);
		fprintf(fd, " ");
	}
	fprintf(fd, "}\n");
}

void imprimeFuncionesTransicion(FILE *fd, AFND *p_afnd){
	int i, j, k, num_simbolos;
	char **simbolos;

	if (!fd || !p_afnd) return;

	// Tomamos los simbolos del alfabeto del automata
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	
	// Recorremos los estados 
	for (i = 0; i < p_afnd->num_estados; i++){
		// Por cada estado, los simbolos del alfabeto
		for (j = 0; j < num_simbolos; j++){
			fprintf(fd, "\n\tf(%s,%s)={", getNombre(p_afnd->estados[i]),
									  simbolos[j]);
			// Y buscamos si existe una transicion con esas entradas
			for (k = 0; k < p_afnd->num_estados; k++)
				if (p_afnd->transiciones[i][j][k] == 1)
					fprintf(fd, " %s", getNombre(p_afnd->estados[k]));
			
		fprintf(fd, " }");
		}
	}
}

void AFNDImprime(FILE * fd, AFND* p_afnd){
	
	if (!fd | !p_afnd) return;

	fprintf(fd, "%s={\n\t", p_afnd->nombre);
	
	// Imprimos el nombre del automata
	AlfabetoImprime(fd, p_afnd->alfabeto);
	fprintf(fd, "\n\t");
	
	// La informacion de los estados
	fprintf(fd, "num_estados = %d\n", p_afnd->num_estados);
	fprintf(fd, "\n\t");
	imprimeEstados(fd, p_afnd);

	// Las transiciones lambda
	fprintf(fd, "\nRL++*={\n");
	imprimeLTransiciones(fd, p_afnd);
	fprintf(fd, "}\n");
	
	// Y la funcion de transicion
	fprintf(fd, "\nFunción de Transición = {\n");
	imprimeFuncionesTransicion(fd, p_afnd);
	fprintf(fd, "\n  }\n");
	fprintf(fd, "}\n");
}

AFND * AFNDInsertaSimbolo(AFND * p_afnd, char * simbolo){

	if (!p_afnd || !simbolo) return NULL;

	AlfabetoInsertaSimbolo(p_afnd->alfabeto, simbolo);
	return p_afnd;
}

AFND * AFNDInsertaTransicion(AFND * p_afnd, 
                             char * nombre_estado_i, 
                             char * nombre_simbolo_entrada, 
                             char * nombre_estado_f ){

	int i, j, flag, k, num_simbolos;
	char **simbolos;

	if (!p_afnd || !nombre_estado_i || ! nombre_simbolo_entrada || !nombre_estado_f)
		return NULL;

	// Tomamos los simbolos del alfabeto del automata
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);

	// Recorremos la lista de estados del automata para encontrar
	// los indices del estado inicial (i) y el final (k)
	i = indiceDeEstado(p_afnd, nombre_estado_i);
	if (i == -1)
		return NULL;

	k = indiceDeEstado(p_afnd, nombre_estado_f);
	if (k == -1)
		return NULL;

	// Recorremos la lista de simbolos para encontrar el indice 
	// del simbolo en la transicion (j)
	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++){
		if (!strcmp(nombre_simbolo_entrada, simbolos[j]))
			flag = 1;
	}
	if (flag == 0)
		return NULL;

	// Modificamos la matriz de transiciones segun los indices
	p_afnd->transiciones[i][--j][k] = 1;
			
	return NULL;
}

AFND * AFNDInsertaLetra(AFND * p_afnd, char * letra){

	if (!p_afnd || !letra) return NULL;
	// Insertamos la letra en la cadena de entrada del automata
	if (!PalabraInsertaLetra(p_afnd->cadena_entrada, letra)) return NULL;

	return p_afnd;
}

AFND * AFNDInsertaEstado(AFND * p_afnd, char * nombre, int tipo){

	if (!p_afnd || !nombre || tipo < 0 || tipo > 3) return NULL;
	if (p_afnd->max_estados <= p_afnd->num_estados) return NULL;

	// Creacion e insercion del estado en la coleccion de estados
	p_afnd->estados[p_afnd->num_estados] = EstadoNuevo(nombre, tipo);
	if (!p_afnd->estados[p_afnd->num_estados]) return NULL;

	p_afnd->num_estados++;
	return p_afnd;
}

void AFNDImprimeConjuntoEstadosActual(FILE * fd, AFND * p_afnd){
	int i;

	if (!p_afnd || !fd) return;

	// Si no hay estados actuales, la cadena se marca como rechazada
	if (!p_afnd->num_estados_actuales){
		fprintf(fd, "CADENA RECHAZADA\n");
		return;
	}

	fprintf(fd, "ACTUALMENTE EN { ");
	// Por cada estado actual, se imprime este
	for (i = 0; i < p_afnd->num_estados_actuales; i++){
		EstadoImprime(fd, p_afnd->estados_actuales[i]);
		fprintf(fd, " ");
	}
	fprintf(fd, "}\n");
}

void AFNDImprimeCadenaActual(FILE *fd, AFND * p_afnd){

	if (!p_afnd || !fd) return;
	imprimeCadena(fd, p_afnd->cadena_entrada);
}

int recursividad_maxima(AFND *p_afnd, Estado **sin_procesar, Estado **procesados, int num_procesados, int num_sin_procesar, int proc){
	int i, j, index, flag;

	if (!p_afnd) return 0;

	// La recursividad para cuando no quedan mas estados actuales que procesar
	if (num_sin_procesar == proc) return num_procesados;

	// Cogemos el indice del primer estado actual sin procesar del automata
	index = indiceDeEstado(p_afnd, getNombre(sin_procesar[num_sin_procesar]));
	// Buscamos sus transiciones lambdas 
	for (i = 0; i < p_afnd->num_estados; i++){
		if (p_afnd->lambdas[index][i] && index != i){
			flag = 0;
			// Comprobamos que la transicion no se encuentre ya entre los estados actuales
			for (j = 0; j < proc && flag == 0; j++)
				if (!strcmp(getNombre(sin_procesar[j]), getNombre(p_afnd->estados[i])))
					flag = 1;
			for (j = 0; j < num_procesados && flag == 0; j++)
				if (!strcmp(getNombre(p_afnd->estados[i]), getNombre(procesados[j])))
					flag = 1;
			// Si no se encuentra entre estos, lo aniadimos a la lista de no procesados
			if (!flag){
				sin_procesar[proc] = p_afnd->estados[i];
				proc ++;
			}
		}
	}

	procesados[num_procesados] = p_afnd->estados[index];

	return recursividad_maxima(p_afnd, sin_procesar, procesados, ++num_procesados, ++num_sin_procesar, proc);
}

AFND * AFNDInicializaEstado (AFND * p_afnd){
	int i;
	Estado **actuales;

	if (!p_afnd) return NULL;

	actuales = (Estado **) malloc(sizeof (Estado *) * p_afnd->num_estados);
	if (!actuales) return NULL;

	p_afnd->num_estados_actuales = 0;
	// Por cada estado, si este es inicial o inicial y final,
	// se añade a la lista de estados actuales
	for (i = 0; i < p_afnd->num_estados; i++){
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL ||
			getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL){
			p_afnd->estados_actuales[p_afnd->num_estados_actuales] = p_afnd->estados[i];
			p_afnd->num_estados_actuales ++;
		}
	}

	p_afnd->num_estados_actuales = recursividad_maxima(p_afnd, p_afnd->estados_actuales, actuales, 0, 0, p_afnd->num_estados_actuales);
	free(p_afnd->estados_actuales);
	p_afnd->estados_actuales = actuales;

	return p_afnd;

}

void reiniciaCadena(AFND *p_afnd){

	if (!p_afnd) return;

	// Elimina y vuelve a crear la cadena de entrada del automata
	PalabraElimina(p_afnd->cadena_entrada);
	p_afnd->cadena_entrada = PalabraNuevo();
}

void AFNDProcesaEntrada(FILE * fd, AFND * p_afnd){

	if (!fd || !p_afnd) return;
	
	// Imprime la situacion inicial del automata
	AFNDImprimeConjuntoEstadosActual(fd, p_afnd);
	imprimeCadena(fd, p_afnd->cadena_entrada);
	fprintf(fd, "\n");
	
	// Procesa el primer simbolo de la cadena de entrada
	p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	
	// Mientras se pueda seguir procesando
	while (p_afnd->simbolo_actual) {
		AFNDTransita(p_afnd);

		AFNDImprimeConjuntoEstadosActual(fd, p_afnd);
		
		// Si no existen estados actuales, se acaba el proceso
		if (p_afnd->num_estados_actuales == 0){
			reiniciaCadena(p_afnd);
			return;
		} 
		
		imprimeCadena(fd, p_afnd->cadena_entrada);
		fprintf(fd, "\n");

		p_afnd->simbolo_actual = procesarSimbolo(p_afnd->cadena_entrada);
	}

	reiniciaCadena(p_afnd);

}
void AFNDTransita(AFND * p_afnd){
	int i, j, k, m, flag;
	int num_simbolos;
	char **simbolos;
	int num_nuevos_estados;
	Estado **nuevos_estados, **estados_lambdas;
	short *transiciones;

	if (!p_afnd) return;

	// Reserva para la nueva coleccion de estados actuales
	nuevos_estados = (Estado **) malloc(p_afnd->num_estados * sizeof(Estado *));
	if (!nuevos_estados) return;
	estados_lambdas = (Estado **) malloc(p_afnd->num_estados * sizeof(Estado *));
	if (!estados_lambdas){
		free(nuevos_estados);
		return;
	}
	num_nuevos_estados = 0;

	// Toma los simbolos del alfabeto del automata
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	simbolos = AlfabetoGetSimbolos(p_afnd->alfabeto);
	
	// Recorre los simbolos para encontrar el indice (j) del simbolo actual
	for (j = 0, flag = 0; j < num_simbolos && flag == 0; j++)
		if (!strcmp(p_afnd->simbolo_actual, simbolos[j]))
			flag = 1;
	j--;

	// Recorre los estados actuales del automata 
	for (k = 0; k < p_afnd->num_estados_actuales; k++){
		// Encuentra, por cada estado actual, su indice dentro de los estados (i)
		for (i = 0, flag = 0; i < p_afnd->num_estados && flag == 0; i++)
			if (!strcmp(getNombre(p_afnd->estados[i]), getNombre(p_afnd->estados_actuales[k])))
				flag = 1;
		i--;
		
		// Toma las transiciones desde el estado con el simbolo actual
		transiciones = p_afnd->transiciones[i][j];
		// Si existe alguna transicion, añade el nuevo estado a la lista de 
		// nuevos estados actuales
		for (m = 0;  m < p_afnd->num_estados; m++)
			if (transiciones[m]){
				nuevos_estados[num_nuevos_estados] = p_afnd->estados[m];
				num_nuevos_estados ++;
			}
	}

	// Asignacion de los nuevos valores
	free(p_afnd->estados_actuales);
	p_afnd->estados_actuales = nuevos_estados;
	p_afnd->num_estados_actuales = num_nuevos_estados;

	// Transiciones lambda
	num_nuevos_estados = 0;
	p_afnd->num_estados_actuales = recursividad_maxima(p_afnd, p_afnd->estados_actuales, estados_lambdas, 0, 0, p_afnd->num_estados_actuales);
	free(p_afnd->estados_actuales);
	p_afnd->estados_actuales = estados_lambdas;

}

// Inserta una transicion lambda
AFND * AFNDInsertaLTransicion(
       AFND * p_afnd, 
       char * nombre_estado_i, 
       char * nombre_estado_f ){

	int i, j;
	
	if (!p_afnd || !nombre_estado_i || !nombre_estado_f) 
		return NULL;

	// Obtenemos indice del estado origen en el array de estados
	i = indiceDeEstado(p_afnd, nombre_estado_i);
	if (i == -1) return NULL;

	// Obtenemos indice del estado destino en el array de estados
	j = indiceDeEstado(p_afnd, nombre_estado_f);
	if (j == -1) return NULL;

	// Marcamos la posicion correspondiente de la matriz de lambdas
	p_afnd->lambdas[i][j] = 1;

	return p_afnd;
}

void cuadradoMatriz(short **matriz_dest, short **matriz_src, int n){
	int i, j, k;

	if (!matriz_dest || !matriz_src) return;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < n; k++)
				if (matriz_src[i][k] == 1 && matriz_src[k][j] == 1)
					matriz_dest[i][j] = 1;

	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++)
			printf("%d ", matriz_dest[i][j]);
		printf("\n");
	}
	printf("\n");
}

void copiaMatriz(short **dest, short **src, int n){
	int i, j;

	if (!dest || !src) return;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			dest[i][j] = src[i][j];
}

AFND * AFNDCierraLTransicion(AFND * p_afnd){
	int i, j, n;
	short **matriz;

	if (!p_afnd) return NULL;

	// Reservamos memoria para otra matriz auxiliar
	matriz = (short **) malloc(sizeof(short *) * p_afnd->num_estados);
	if (!matriz) return NULL;

	// La inicializamos con los valores de las relaciones lambda
	for (i = 0; i < p_afnd-> num_estados; i ++){
		matriz[i] = (short *) malloc(sizeof(short) * p_afnd->num_estados);
		if (!matriz[i]){
			for (i--; i >= 0; i--)
				free(matriz[i]);
			free(matriz);
			return NULL;
		}
		for (j = 0; j < p_afnd->num_estados; j++)
			matriz[i][j] = p_afnd->lambdas[i][j];
	}

	// Elevamos la matriz de lambdas al cuadrado hasta que la potencia sea mayor que el numero de estados
	// Esto funciona por la propiedad reflexiva
	// Si no, habria que hacer las potencias de una en una y hacer OR con el resultado anterior
	// El objetivo es llegar hasta la potencia en la que todos los elementos de la matriz
	//		sean 0, o todos sean 1, o sea igual que la del paso anterior. En cualquiera de las tres
	// 		condiciones de parada, no importa si nos pasamos, ya que lo peor que puede pasar es que se quede igual.
	// Cada vez que aumentamos de potencia tenemos en cuenta un nivel más de lejania.
	// Como solo hay n estados, no puede haber dos estados con lejania mayor que n, por lo que siempre es suficiente
	// 		elevar la matriz a n.
	for (n = 1; n <= p_afnd->num_estados; n*=2){
		cuadradoMatriz(matriz, p_afnd->lambdas, p_afnd->num_estados);
		copiaMatriz(p_afnd->lambdas, matriz, p_afnd->num_estados);
	}

	for (i = 0; i < p_afnd->num_estados; i++)
		free(matriz[i]);
	free(matriz);

	return p_afnd;
}

AFND * AFNDInicializaCadenaActual (AFND * p_afnd ){

	if (!p_afnd) return NULL;

	return p_afnd;

}

void imprimeLTransiciones(FILE *fd, AFND *p_afnd){
	int i, j;

	if  (!fd || !p_afnd) return;

	fprintf(fd, "\t   ");
	for (i = 0; i < p_afnd->num_estados; i++)
		fprintf(fd, "[%d] ", i);

	fprintf(fd, "\n");
	for (i = 0; i < p_afnd->num_estados; i++){
		fprintf(fd, "\t[%d] ", i);
		for (j = 0; j < p_afnd->num_estados; j++)
			fprintf(fd, "%d   ", p_afnd->lambdas[i][j]);
		fprintf(fd, "\n");
	}

}

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

	int num_estados, i, j, k;
	int num_simbolos;
	AFND *afnd1O;

	if (!p_afnd) return NULL;

	num_estados = p_afnd->num_estados;
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd->alfabeto);
	afnd1O = AFNDNuevo(p_afnd->nombre, p_afnd->num_estados+2, num_simbolos);
	if (!afnd1O) return NULL;

	AFNDInsertaEstado(afnd1O, "nuevo_q0", INICIAL);
	AFNDInsertaEstado(afnd1O, "nuevo_qf", FINAL);

	for(i = 2; i < num_estados; i++)
		for(j = 0; j < num_simbolos; j++)
			for(k = 2; k < num_estados; k++)
				afnd1O->transiciones[i][j][k] = p_afnd->transiciones[i][j][k];
	
	for(i = 2; i < num_estados; i++)
		for(j = 2; j < num_estados; j++)
			afnd1O->lambdas[i][j] = p_afnd->lambdas[i][j];

	for(i = 0; i < num_estados; i++){
		AFNDInsertaEstado(afnd1O, getNombre(p_afnd->estados[i]), NORMAL);
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL)
			AFNDInsertaLTransicion(afnd1O, "nuevo_q0", getNombre(p_afnd->estados[i]));
		if (getTipoEstado(p_afnd->estados[i]) == FINAL ||
			  getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL)
			AFNDInsertaLTransicion(afnd1O, getNombre(p_afnd->estados[i]), "nuevo_qf");
	}

	return afnd1O;

}

// SI ALGO PETA MIRAR FINES DE CADENAS
AFND *AFND1OUne(AFND *p_afnd1O_1, AFND *p_afnd1O_2){

	int num_estados1, num_estados2, len;
	int num_estados, num_simbolos;
	int i, j, k;
	AFND *afnd1O;
	char *nombre, *nombre1, *nombre2;

	if (!p_afnd1O_1 || !p_afnd1O_2) return NULL;

	num_estados1 = p_afnd1O_1->num_estados;
	num_estados2 = p_afnd1O_2->num_estados;
	len = strlen(p_afnd1O_1->nombre) + strlen(p_afnd1O_2->nombre) + 5;

	nombre = (char *) malloc(sizeof(char) * len);
	if (!nombre) return NULL;

	nombre2 = (char *) malloc(sizeof(char) * len);
	if (!nombre2){
		free(nombre);
	 	return NULL;
	}

	nombre1 = (char *) malloc(sizeof(char) * len);
	if (!nombre1){
		free(nombre);
		free(nombre2);
	 	return NULL;
	}

	if (strcpy(nombre, p_afnd1O_1->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return  NULL;
	}

	if (strcpy(nombre1, p_afnd1O_1->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return NULL;
	}

	if (strcpy(nombre2, p_afnd1O_2->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return NULL;
	}

	nombre1 = strcat(nombre1, "_");
	nombre2 = strcat(nombre2, "_");


	num_estados = p_afnd1O_1->num_estados + p_afnd1O_2->num_estados + 2;
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd1O_1->alfabeto) + AlfabetoGetNumSimbolos(p_afnd1O_2->alfabeto);
	afnd1O = AFNDNuevo(strcat(strcat(nombre, "_"), p_afnd1O_2->nombre), 
										 num_estados, 
										 num_simbolos);
	
	if (!afnd1O){
		free(nombre);
		free(nombre1);
		free(nombre2);
		return NULL;
	}

	AFNDInsertaEstado(afnd1O, strcat(nombre, "_q0"), INICIAL);
	nombre[strlen(nombre)-1] = 'f';
	AFNDInsertaEstado(afnd1O, nombre, FINAL);

	for(i = 2; i < num_estados; i++){
		for(j = 0; j < num_simbolos; j++){
			for(k = 2; k < num_estados; k++){
				if (i < num_estados1 && k < num_estados1)
					afnd1O->transiciones[i][j][k] = p_afnd1O_1->transiciones[i][j][k];
				else
					afnd1O->transiciones[i][j][k] = p_afnd1O_2->transiciones[i-num_estados1][j][k-num_estados1];
			}
		}
	}
	
	for(i = 2; i < num_estados; i++){
		for(j = 2; j < num_estados; j++){
			if (i < num_estados1 && j < num_estados1)
				afnd1O->lambdas[i][j] = p_afnd1O_1->lambdas[i][j];
			else
				afnd1O->lambdas[i][j] = p_afnd1O_2->lambdas[i-num_estados1][j-num_estados1];
		}
	}

	for(i = 0; i < num_estados1; i++){
		nombre1[strlen(p_afnd1O_1->nombre)] = '\0';
		AFNDInsertaEstado(afnd1O, strcat(nombre1, getNombre(p_afnd1O_1->estados[i])), NORMAL);
		if (getTipoEstado(p_afnd1O_1->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd1O_1->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = '0';
			AFNDInsertaLTransicion(afnd1O, nombre, nombre1);
		}
		if (getTipoEstado(p_afnd1O_1->estados[i]) == FINAL ||
			  getTipoEstado(p_afnd1O_1->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = 'f';
			AFNDInsertaLTransicion(afnd1O, nombre1, nombre);
		}
	}

	for(i = 0 ; i < num_estados2; i++){
		nombre2[strlen(p_afnd1O_2->nombre)] = '\0';
		AFNDInsertaEstado(afnd1O, strcat(nombre2, getNombre(p_afnd1O_2->estados[i])), NORMAL);
		if (getTipoEstado(p_afnd1O_2->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd1O_2->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = '0';
			AFNDInsertaLTransicion(afnd1O, nombre, nombre2);
		}
		if (getTipoEstado(p_afnd1O_2->estados[i]) == FINAL ||
			  getTipoEstado(p_afnd1O_2->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = 'f';
			AFNDInsertaLTransicion(afnd1O, nombre2, nombre);
		}
	}

	return afnd1O;
}

// SI ALGO PETA MIRAR FINES DE CADENAS
AFND *AFND1OConcatena(AFND *p_afnd_origen1, AFND *p_afnd_origen2){
	int num_estados1, num_estados2, len;
	int num_estados, num_simbolos;
	int i, j, k;
	AFND *afnd1O;
	char *nombre, *nombre1, *nombre2;
	
	if (!p_afnd_origen1 || !p_afnd_origen2) return NULL;

	num_estados1 = p_afnd_origen1->num_estados;
	num_estados2 = p_afnd_origen2->num_estados;
	len = strlen(p_afnd_origen1->nombre) + strlen(p_afnd_origen2->nombre) + 5;

	nombre = (char *) malloc(sizeof(char) * len);
	if (!nombre) return NULL;

	nombre2 = (char *) malloc(sizeof(char) * len);
	if (!nombre2){
		free(nombre);
	 	return NULL;
	}

	nombre1 = (char *) malloc(sizeof(char) * len);
	if (!nombre1){
		free(nombre);
		free(nombre2);
	 	return NULL;
	}

	if (strcpy(nombre, p_afnd_origen1->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return  NULL;
	}

	if (strcpy(nombre1, p_afnd_origen1->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return NULL;
	}

	if (strcpy(nombre2, p_afnd_origen2->nombre) < 0){
		free(nombre);
		free(nombre2);
		free(nombre1);
		return NULL;
	}

	nombre1 = strcat(nombre1, "_");
	nombre2 = strcat(nombre2, "_");


	num_estados = p_afnd_origen1->num_estados + p_afnd_origen2->num_estados + 2;
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd_origen1->alfabeto) + AlfabetoGetNumSimbolos(p_afnd_origen2->alfabeto);
	afnd1O = AFNDNuevo(strcat(strcat(nombre, "_"), p_afnd_origen2->nombre), 
										 num_estados, 
										 num_simbolos);
	
	if (!afnd1O){
		free(nombre);
		free(nombre1);
		free(nombre2);
		return NULL;
	}

	AFNDInsertaEstado(afnd1O, strcat(nombre, "_q0"), INICIAL);
	nombre[strlen(nombre)-1] = 'f';
	AFNDInsertaEstado(afnd1O, nombre, FINAL);

	for(i = 2; i < num_estados; i++){
		for(j = 0; j < num_simbolos; j++){
			for(k = 2; k < num_estados; k++){
				if (i < num_estados1 && k < num_estados1)
					afnd1O->transiciones[i][j][k] = p_afnd_origen1->transiciones[i][j][k];
				else
					afnd1O->transiciones[i][j][k] = p_afnd_origen2->transiciones[i-num_estados1][j][k-num_estados1];
			}
		}
	}
	
	for(i = 2; i < num_estados; i++){
		for(j = 2; j < num_estados; j++){
			if (i < num_estados1 && j < num_estados1)
				afnd1O->lambdas[i][j] = p_afnd_origen1->lambdas[i][j];
			else
				afnd1O->lambdas[i][j] = p_afnd_origen2->lambdas[i-num_estados1][j-num_estados1];
		}
	}

	for(i = 0; i < num_estados1; i++){
		nombre1[strlen(p_afnd_origen1->nombre)] = '\0';
		AFNDInsertaEstado(afnd1O, strcat(nombre1, getNombre(p_afnd_origen1->estados[i])), NORMAL);
		if (getTipoEstado(p_afnd_origen1->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd_origen1->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = '0';
			AFNDInsertaLTransicion(afnd1O, nombre, nombre1);
		}
	}

	for(i = 0 ; i < num_estados2; i++){
		nombre2[strlen(p_afnd_origen2->nombre)] = '\0';
		AFNDInsertaEstado(afnd1O, strcat(nombre2, getNombre(p_afnd_origen2->estados[i])), NORMAL);
		if (getTipoEstado(p_afnd_origen2->estados[i]) == FINAL ||
			  getTipoEstado(p_afnd_origen2->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = 'f';
			AFNDInsertaLTransicion(afnd1O, nombre2, nombre);
		}
	}

	return afnd1O;
}

AFND *AFND1OEstrella(AFND *p_afnd_origen){
	int num_estados, i, j, k;
	int num_simbolos;
	AFND *afnd1O;
	char *nombre, *nombre2;

	if (!p_afnd_origen)

	nombre = (char *) malloc(sizeof(char) * (strlen(p_afnd_origen->nombre) + 10));
	if (!nombre) return NULL;

	nombre2 = (char *) malloc(sizeof(char) * (strlen(p_afnd_origen->nombre) + 10));
	if (!nombre2){
		free(nombre);
		return NULL;
	}

	if (strcpy(nombre, p_afnd_origen->nombre) < 0){
		free(nombre);
		free(nombre2);
		return NULL;
	}

	if (strcpy(nombre2, p_afnd_origen->nombre) < 0){
		free(nombre);
		free(nombre2);
		return NULL;
	}

	strcat(nombre2, "_nuevo_q0");

	num_estados = p_afnd_origen->num_estados;
	num_simbolos = AlfabetoGetNumSimbolos(p_afnd_origen->alfabeto);
	afnd1O = AFNDNuevo(strcat(nombre, "_estrella"), p_afnd_origen->num_estados+2, num_simbolos);
	if (!afnd1O){
		free(nombre);
		return NULL;
	}

	nombre[strlen(p_afnd_origen->nombre)] = '\0';
	AFNDInsertaEstado(afnd1O, strcat(nombre, "nuevo_q0"), INICIAL);
	nombre[strlen(nombre)-1] = 'f';
	AFNDInsertaEstado(afnd1O, nombre, FINAL);

	for(i = 2; i < num_estados; i++)
		for(j = 0; j < num_simbolos; j++)
			for(k = 2; k < num_estados; k++)
				afnd1O->transiciones[i][j][k] = p_afnd_origen->transiciones[i][j][k];
	
	for(i = 2; i < num_estados; i++)
		for(j = 2; j < num_estados; j++)
			afnd1O->lambdas[i][j] = p_afnd_origen->lambdas[i][j];

	for(i = 0; i < num_estados; i++){
		AFNDInsertaEstado(afnd1O, getNombre(p_afnd_origen->estados[i]), NORMAL);
		if (getTipoEstado(p_afnd_origen->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd_origen->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = '0';
			AFNDInsertaLTransicion(afnd1O, nombre, getNombre(p_afnd_origen->estados[i]));
		}
		if (getTipoEstado(p_afnd_origen->estados[i]) == FINAL ||
			  getTipoEstado(p_afnd_origen->estados[i]) == INICIAL_Y_FINAL){
			nombre[strlen(nombre)-1] = 'f';
			AFNDInsertaLTransicion(afnd1O, getNombre(p_afnd_origen->estados[i]), nombre);
		}
	}

	nombre[strlen(nombre)-1] = 'f';
	AFNDInsertaLTransicion(afnd1O, nombre, nombre2);
	AFNDInsertaLTransicion(afnd1O, nombre2, nombre);

	return afnd1O;
}

void AFNDADot(AFND *p_afnd){
	char *file_name;
	FILE *f;
	int i, j, k;
	
	if (!p_afnd) return;

	file_name = (char *) malloc(sizeof(char *) * (strlen(p_afnd->nombre) + 5));
	if (!file_name) return;

	if (strcpy(file_name, p_afnd->nombre) < 0){
		free(file_name);
		return;
	}

	if (!strcat(file_name, ".dot")){
		free(file_name);
		return;
	}

	f = fopen(file_name, "w+");
	if (!f) {
		free(file_name);
		return;
	}

	fprintf(f, "digraph %s { rankdir=BT;\n", p_afnd->nombre);
	fprintf(f, "\t _invisible [style=\"invis\"];\n");

	for (i = 0; i < p_afnd->num_estados; i++){
		fprintf(f, "\t %s;", getNombre(p_afnd->estados[i]));	
		if (getTipoEstado(p_afnd->estados[i]) == FINAL || 
				getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL)
			fprintf(f, "[penwidth=\"2\"]");
		fprintf(f, "\n");
	}

	for (i = 0; i < p_afnd->num_estados; i++){
		if (getTipoEstado(p_afnd->estados[i]) == INICIAL || 
				getTipoEstado(p_afnd->estados[i]) == INICIAL_Y_FINAL){
			fprintf(f, "_invisible -> %s\n", getNombre(p_afnd->estados[i]));
		}
	}	

	for (i = 0; i < p_afnd->num_estados; i++)
		for (j = 0; j < AlfabetoGetNumSimbolos(p_afnd->alfabeto); j++)
			for (k = 0; k < p_afnd->num_estados; k++)
					if (p_afnd->transiciones[i][j][k] == 1)
						fprintf(f, "%s -> %s [label=\"%s\"];\n", 
							getNombre(p_afnd->estados[i]),
						  getNombre(p_afnd->estados[k]),
						  AlfabetoGetSimbolos(p_afnd->alfabeto)[k]);
		

	for (i = 0; i < p_afnd->num_estados; i++)
			for (j = 0; j < p_afnd->num_estados; j++)
					if (p_afnd->lambdas[i][j])
						fprintf(f, "%s -> %s [label=\"&lambda;\"];\n", 
							getNombre(p_afnd->estados[i]),
						  getNombre(p_afnd->estados[j]));
	
	fprintf(f, "}\n");
	
	fclose(f);	
	free(file_name);

	return;

}
