//
// Created by jhair on 5/11/19.
//

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "vmsim.h"
#include "options.h"
#include "fault.h"
#include "util.h"

#define MIN_PHYS_PAGES   3
#define MIN_PAGESIZE    16

/* opts significa options, referencia a las opciones que se escriben
 * en la terminal al momento de ejecutar el programa */

/* verbose indica que se imprimirán avisos en la terminal de lo que se esté procesando:
 * [ERROR]:
 * [INFO ]:
 * etc
 * obs: los mensajes por error estándar han sido modificados, usando ahora corchetes para
 * describir qué tipo de mensaje son y de dónde provienen (nombre del archivo en donde se
 * están ejecutando)
 * */

/* Estructura de opciones globales, la función processOptions()
 * colocará los valores ingresados por consola en la estructura */
opts_t chosenopts;

static const char *shortopts = "hvtVp:s:l:o:";

/**********************************************************************/
/* Handle systems without GNU libc-style longopt support              */
#ifdef HAVE_GETOPT_LONG
    #define __GNU_SOURCE
    #include <getopt.h>
    static const struct option longopts[] = {
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"verbose", no_argument, NULL, 'v'},
        {"test",    no_argument, NULL, 't'},
        {"output",  no_argument, NULL, 'o'},
        {"limit",   no_argument, NULL, 'l'},
        {"pages",   no_argument, NULL, 'p'},
        {"size",    no_argument, NULL, 's'},
        {0, 0, 0, 0}
    };
    #define GETOPT(argc, argv) getopt_long(argc, argv, shotopts, longopts, NULL);
#else
    #define GETOPT(argc, argv) getopt(argc, argv, shortopts);
#endif
/**********************************************************************/

static void optionsHandleAlgorithm(const char *algorithmName);
static long optionsAtoi(const char *argument);
static void optionsPrintHelp();
static void optionsPrintVersion();
static char *_longopt(char *longOptHelp);
static char *_zlibhelp();
static void _algorithmHelp();

/* FUNCIONES PROPIAS */
static void optionsHandleArgumentsValidation();
static void optionsHandleInputFileArgument(int argc, char **argv);
static void optionsHandleOutputFileArgument();

/* Procesa el arreglo argc/argv, actualizando la estructura global
 * de opciones 'chosenopts'*/
void processOptions(int argc, char **argv) {
    int option;
    int help = FALSE, version = FALSE;

    chosenopts.outputFile    = NULL;
    chosenopts.inputFile     = NULL;
    chosenopts.verbose       = FALSE;
    chosenopts.test          = FALSE;
    chosenopts.pageSize      = 1024;
    chosenopts.physicalPages = 128;
    chosenopts.limit         = 0;

    /* macros GETOPT obtiene la clave de la opción ingresada como argumentos en la línea de comandos,
     * de acuerdo a los caracteres entregados en la variable global 'shortopts'. Si no hay más argumentos
     * por leer, se obtiene un -1, lo cual acaba con el bucle.
     * Info de cómo funciona getopt():
     * https://www.geeksforgeeks.org/getopt-function-in-c-to-parse-command-line-arguments/ */
    while (TRUE) {
        option = GETOPT(argc, argv);
        if (option == -1) break;

        switch (option) {
            case 'h': help = TRUE; break;
            case 'V': version = TRUE; break;
            case 'v': chosenopts.verbose = TRUE; break;
            case 'o': chosenopts.outputFile = optarg; break;
            case 'l': chosenopts.limit = optionsAtoi(optarg); break;
            case 't': chosenopts.test = TRUE;
            case 'p': chosenopts.physicalPages = (int)optionsAtoi(optarg); break;
            case 's': chosenopts.pageSize = (int)optionsAtoi(optarg); break;
            case '?': help = TRUE; break;
            default:
                printf("'getopt' retornó '%c' - opción no manejable\n", option);
                abort();
        }
    } // fin del bucle, no hay más argumentos como opciones por leer

    if (version) {
        optionsPrintVersion();
        if (help) printf("\n");
    }

    if (help) {
        optionsPrintHelp();
        exit(0);
    }

    // función propia, para mayor orden dentro de esta función principal de la librería options.h
    optionsHandleArgumentsValidation();

    // optind, a estas alturas, apunta al último argumento, el cual es el nombre del algoritmo a usar
    optionsHandleAlgorithm(argv[optind]);

    // función propia, encargada de setear el nombre del archivo de entrada estándar, imprime nombre si verbose es TRUE
    optionsHandleInputFileArgument(argc, argv);

    // función propia, encargada de imprimir en pantalla el nombre del archivo de salida, si opción verbose es TRUE
    optionsHandleOutputFileArgument();
}

void optionsHandleArgumentsValidation() {
    if (chosenopts.limit < 0) {
        fprintf(stderr, "[ERROR-OPTIONS]: límite debe ser > 0\n");
        exit(1);
    }

    if (chosenopts.physicalPages < MIN_PHYS_PAGES) {
        fprintf(stderr, "[ERROR-OPTIONS]: número de páginas físicas debe ser al menos %d\n", MIN_PHYS_PAGES);
        exit(1);
    }

    if (chosenopts.pageSize < MIN_PAGESIZE) {
        fprintf(stderr, "[ERROR-OPTIONS]: tamaño de página debe ser al menos %d bytes\n", MIN_PAGESIZE);
        exit(1);
    }
    if (log2(chosenopts.pageSize) == -1) {
        fprintf(stderr, "[ERROR-OPTIONS]: tamaño de la página debe ser potencia de 2\n");
        exit(1);
    }

    /* La variable 'optind' es el índice del siguiente elemento dentro del vector argv[] a ser procesado.
     * Este es inicializado a 1 por el sistema, y la función getopt() lo actualiza cuando finaliza con cada
     * elemento de argv[]. Dentro de la línea de argumentos, el último valor ingresado es el nombre del algoritmo
     * a usar. Si 'optind' indica un índice mayor a lo registrado por argc, entonces el usuario no ha ingresado
     * el nombre del algoritmo y se arroja un error.
     * Info sobre funcionamiento de optind: http://linux.die.net/man/3/optind */

    if (optind >= argc) {
        fprintf(stderr, "[ERROR-OPTIONS]: el algoritmo a usar debe ser específicado\n");
        exit(1);
    }
}

void optionsHandleInputFileArgument(int argc, char **argv) {
    // si existe un argumento más por leer, es el valor a asignar a la variable archivo de entrada 'inputFile'
    if (optind+1 < argc) {
        chosenopts.inputFile = argv[optind+1];
    }

    // caracter '-' como valor recibido para input file indica que debe ser leído desde la entrada estándar
    if (chosenopts.inputFile == NULL || strcmp("-", chosenopts.inputFile) == 0) {
        chosenopts.inputFile = NULL;
        if (chosenopts.verbose) printf("[INFO -OPTIONS]: leyendo desde entrada estándar (stdin)\n");
    }
    else if (chosenopts.verbose) {
        printf("[INFO -OPTIONS]: leyendo desde %s\n", chosenopts.inputFile);
    }
}

void optionsHandleOutputFileArgument() {
    if (chosenopts.verbose && chosenopts.outputFile) {
        printf("[INFO -OPTIONS]: enviando salida (output) a %s\n", chosenopts.outputFile);
    }
}

long optionsAtoi(const char *argument) {
    char *end;
    long convertedNumber;

    // string to long en base=10, si no es válido, end apunta a un caracter vacío
    convertedNumber = strtol(argument, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "[ERROR-OPTIONS]: argumento tipo entero inválido = %s\n", argument);
    }

    return convertedNumber;
}

void optionsHandleAlgorithm(const char *algorithmName) {
    // búsqueda del algoritmo escogido usando el nombre ingresado
    fault_handler_info_t *algorithm;
    for (algorithm=fault_handlers; algorithm->name != NULL; algorithm++) {
        if (strcmp(algorithm->name, algorithmName) == 0) break;
    }

    if (algorithm->name == NULL) {
        fprintf(stderr, "[ERROR-OPTIONS]: No existe ningún algoritmo disponible con el nombre '%s'\n", algorithmName);
        exit(1);
    }
    else if (chosenopts.verbose) {
        printf("[INFO -OPTIONS]: Usando algoritmo elegido '%s'\n", algorithmName);
    }

    chosenopts.faultHandler = algorithm;
}

void optionsPrintVersion() {
    printf("Package version 1.01 \n");
    printf("Report bugs to rick@cs.washington.edu\n");
    printf("Available under the GNU General Public License\n");

    printf("\n\n");
    printf("Modified package version 1.00 \n");
    printf("Modified by Jhair Guzman, Github account: @DanielGuzmanT\n");
    printf("Available under the GNU General Public License\n");
}

void optionsPrintHelp() {

}

char *_longopt(char *longOptHelp) {
    return NULL;
}

char *_zlibhelp() {
    return NULL;
}

void _algorithmHelp() {

}


