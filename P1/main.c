// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>


// #include "afnd.h"

// /* ESTOS INCLUDES DEPENDEN DE LA IMPLEMENTACIÓN, TAL VEZ TÚ DISEÑES OTROS MÓDULOS */
// #include "alfabeto.h"
// #include "estado.h"
// #include "palabra.h"


// int main(int argc, char ** argv)
// {

// /* DECLARACIÓN DE UN PUNTERO A UN AFND */
//     AFND * p_afnd;


// /* INICIALIZACIÓN DE UN NUEVO AFND DE NOMBRE af1 Y CON 3 ESTADOS Y 2 SÍMBOLOS EN SU ALFABETO */
//     p_afnd = AFNDNuevo("af1",3,2);
    

// /* DEFINICIÓN DEL ALFABETO DEL AFND */
//     AFNDInsertaSimbolo(p_afnd,"0");
//     AFNDInsertaSimbolo(p_afnd,"1");

// /* DEFINICIÓN DEL CONJUNTO DE ESTADOS */
//     AFNDInsertaEstado(p_afnd,"q0",INICIAL);
//     AFNDInsertaEstado(p_afnd,"q1",NORMAL);
//     AFNDInsertaEstado(p_afnd,"q2",FINAL);

// /* DEFINICIÓN DE LAS TRANSICIONES NO LAMBDA */
//     AFNDInsertaTransicion(p_afnd, "q0", "1", "q1");
//     // AFNDInsertaTransicion(p_afnd, "q0", "1", "q0");
//     AFNDInsertaTransicion(p_afnd, "q1", "1", "q1");
//     AFNDInsertaTransicion(p_afnd, "q1", "0", "q2");

// /* SE MUESTRA EL AFND DEFINIDO */
//     fprintf(stdout,"\n****************** AFND *********************\n");
//     AFNDImprime(stdout,p_afnd);
//     fprintf(stdout,"\n*********************************************\n");

// /* DEFINICIÓN DE LA CADENA DE ENTRADA [ 0 1 0 1 1 ] */
//     p_afnd= AFNDInsertaLetra(p_afnd,"0");
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");
//     p_afnd= AFNDInsertaLetra(p_afnd,"0");
//     // p_afnd= AFNDInsertaLetra(p_afnd,"1");

//  SE ESTABLECE COMO ESTADO ACTUAL DEL AUTÓMATA EL INICIAL 

//     AFNDInicializaEstado (p_afnd);

// /* SE MUESTRA LA CADENA ACTUAL */

//     fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
//     AFNDImprimeCadenaActual(stdout,p_afnd);
//     fprintf(stdout,"\n*********************************************\n");

// /* SE PROCESA LA CADENA DE ENTRADA ACTUAL MOSTRANDO UNA TRAZA DEL FUNCIONAMIENTO DEL AUTOMATA: EN CADA PASO DE ANÁLISIS SE MUESTRA LA CADENA ACTUAL Y EL CONJUNTO DE ESTADOS EN LOS QUE SE ENCUENTRA EL AUTÓMATA */

//     AFNDProcesaEntrada(stdout,p_afnd);


// /* DEFINICIÓN DE LA CADENA DE ENTRADA [ 0 1 1 0 0 ] */
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");
//     p_afnd= AFNDInsertaLetra(p_afnd,"0");
//     p_afnd= AFNDInsertaLetra(p_afnd,"1");

// /* SE ESTABLECE COMO ESTADO ACTUAL DEL AUTÓMATA EL INICIAL */

//     AFNDInicializaEstado (p_afnd);

// /* SE MUESTRA LA CADENA ACTUAL */

//     fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
//     AFNDImprimeCadenaActual(stdout,p_afnd);
//     fprintf(stdout,"\n*********************************************\n");


// /* SE PROCESA LA CADENA DE ENTRADA ACTUAL MOSTRANDO UNA TRAZA DEL FUNCIONAMIENTO DEL AUTOMATA: EN CADA PASO DE ANÁLISIS SE MUESTRA LA CADENA ACTUAL Y EL CONJUNTO DE ESTADOS EN LOS QUE SE ENCUENTRA EL AUTÓMATA */

//     AFNDProcesaEntrada(stdout,p_afnd);

// /* SE LIBERAN TODOS LOS RECURSOS ASOCIADOS CON EL AFND */
//     AFNDElimina(p_afnd);

//     return 0;

// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "alfabeto.h"
#include "estado.h"
#include "afnd.h"
#include "palabra.h"


int main(int argc, char ** argv)
{

    AFND * p_afnd_l;



    p_afnd_l = AFNDNuevo("afl1",8,2);

    AFNDInsertaSimbolo(p_afnd_l,"0");
    AFNDInsertaSimbolo(p_afnd_l,"1");

    AFNDInsertaEstado(p_afnd_l,"q0",INICIAL);
    AFNDInsertaEstado(p_afnd_l,"q1",NORMAL);
    AFNDInsertaEstado(p_afnd_l,"q2",NORMAL);
    AFNDInsertaEstado(p_afnd_l,"q3",NORMAL);
    AFNDInsertaEstado(p_afnd_l,"q4",FINAL);
    AFNDInsertaEstado(p_afnd_l,"q5",NORMAL);
    AFNDInsertaEstado(p_afnd_l,"q6",NORMAL);
    AFNDInsertaEstado(p_afnd_l,"q7",FINAL);


    AFNDInsertaTransicion(p_afnd_l, "q1", "1", "q1");
    AFNDInsertaTransicion(p_afnd_l, "q2", "0", "q3");
    AFNDInsertaTransicion(p_afnd_l, "q3", "0", "q4");
    AFNDInsertaTransicion(p_afnd_l, "q5", "1", "q6");
    AFNDInsertaTransicion(p_afnd_l, "q6", "1", "q7");
    AFNDInsertaTransicion(p_afnd_l, "q7", "0", "q7");
    AFNDInsertaTransicion(p_afnd_l, "q7", "1", "q7");


    AFNDInsertaLTransicion(p_afnd_l, "q1", "q2");
    AFNDInsertaLTransicion(p_afnd_l, "q2", "q3");
    AFNDInsertaLTransicion(p_afnd_l, "q3", "q0");
    AFNDInsertaLTransicion(p_afnd_l, "q0", "q4");

    

    AFNDCierraLTransicion(p_afnd_l);

    AFNDImprime(stdout,p_afnd_l);

    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");


    p_afnd_l = AFNDInicializaEstado(p_afnd_l);

    fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
    AFNDImprimeCadenaActual(stdout,p_afnd_l);
    fprintf(stdout,"\n*********************************************\n");

    AFNDProcesaEntrada(stdout,p_afnd_l);

/*********************************************************************************/

    p_afnd_l = AFNDInicializaCadenaActual(p_afnd_l);
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");

    p_afnd_l = AFNDInicializaEstado(p_afnd_l);

    fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
    AFNDProcesaEntrada(stdout,p_afnd_l);
    fprintf(stdout,"\n*********************************************\n");


/*********************************************************************************/

    p_afnd_l = AFNDInicializaCadenaActual(p_afnd_l);
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"0");

    p_afnd_l = AFNDInicializaEstado(p_afnd_l);

    fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
    AFNDProcesaEntrada(stdout,p_afnd_l);
    fprintf(stdout,"\n*********************************************\n");


/*********************************************************************************/

    p_afnd_l = AFNDInicializaCadenaActual(p_afnd_l);
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"1");
    AFNDInsertaLetra(p_afnd_l,"0");
    AFNDInsertaLetra(p_afnd_l,"1");

    p_afnd_l = AFNDInicializaEstado(p_afnd_l);

    fprintf(stdout,"\n**************** PROCESA CADENA *************\n");
    AFNDProcesaEntrada(stdout,p_afnd_l);
    fprintf(stdout,"\n*********************************************\n");


/*********************************************************************************/





    AFNDElimina(p_afnd_l);

/********************************************************************************/


    return 0;

}
