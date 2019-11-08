//
// Created by jhair on 5/11/19.
//

#include <unistd.h>
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
opts_t chosenOpts;

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
static char *longOpt(char *longoptHelp);
static char *showIfZlibAvailable();
static void listAlgorithmsHelp();

/* FUNCIONES PROPIAS */
static void optionsHandleArgumentsValidation(int argc);
static void optionsHandleInputFileArgument(int argc, char **argv);
static void optionsHandleOutputFileArgument();

/* Procesa el arreglo argc/argv, actualizando la estructura global
 * de opciones 'chosenOpts'*/
void processOptions(int argc, char **argv) {
    int option;
    int help = FALSE, version = FALSE;

    chosenOpts.outputFile    = NULL;
    chosenOpts.inputFile     = NULL;
    chosenOpts.verbose       = FALSE;
    chosenOpts.test          = FALSE;
    chosenOpts.pageSize      = 1024;
    chosenOpts.physicalPages = 128;
    chosenOpts.limit         = 0;

    /* macros GETOPT obtiene la clave de la opción ingresada como argumentos en la línea de comandos,
     * de acuerdo a los caracteres entregados en la variable global 'shortopts'. Si no hay más argumentos
     * por leer, se obtiene un -1, lo cual acaba con el bucle.
     * Info de cómo funciona getopt():
     * https://www.geeksforgeeks.org/getopt-function-in-c-to-parse-command-line-arguments/ */
    while (TRUE) {
        option = GETOPT(argc, argv)
        if (option == -1) break;

        switch (option) {
            case 'h': help = TRUE; break;
            case 'V': version = TRUE; break;
            case 'v': chosenOpts.verbose = TRUE; break;
            case 'o': chosenOpts.outputFile = optarg; break;
            case 'l': chosenOpts.limit = optionsAtoi(optarg); break;
            case 't': chosenOpts.test = TRUE;
            case 'p': chosenOpts.physicalPages = (int)optionsAtoi(optarg); break;
            case 's': chosenOpts.pageSize = (int)optionsAtoi(optarg); break;
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
    optionsHandleArgumentsValidation(argc);

    // optind, a estas alturas, apunta al último argumento, el cual es el nombre del algoritmo a usar
    optionsHandleAlgorithm(argv[optind]);

    // función propia, encargada de setear el nombre del archivo de entrada estándar, imprime nombre si verbose es TRUE
    optionsHandleInputFileArgument(argc, argv);

    // función propia, encargada de imprimir en pantalla el nombre del archivo de salida, si opción verbose es TRUE
    optionsHandleOutputFileArgument();
}

void optionsHandleArgumentsValidation(int argc) {
    if (chosenOpts.limit < 0) {
        fprintf(stderr, "[ERROR-OPTIONS]: límite debe ser > 0\n");
        exit(1);
    }

    if (chosenOpts.physicalPages < MIN_PHYS_PAGES) {
        fprintf(stderr, "[ERROR-OPTIONS]: número de páginas físicas debe ser al menos %d\n", MIN_PHYS_PAGES);
        exit(1);
    }

    if (chosenOpts.pageSize < MIN_PAGESIZE) {
        fprintf(stderr, "[ERROR-OPTIONS]: tamaño de página debe ser al menos %d bytes\n", MIN_PAGESIZE);
        exit(1);
    }
    if (log_2(chosenOpts.pageSize) == -1) {
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
        chosenOpts.inputFile = argv[optind + 1];
    }

    // caracter '-' como valor recibido para input file indica que debe ser leído desde la entrada estándar
    if (chosenOpts.inputFile == NULL || strcmp("-", chosenOpts.inputFile) == 0) {
        chosenOpts.inputFile = NULL;
        if (chosenOpts.verbose) printf("[INFO -OPTIONS]: leyendo desde entrada estándar (stdin)\n");
    }
    else if (chosenOpts.verbose) {
        printf("[INFO -OPTIONS]: leyendo desde %s\n", chosenOpts.inputFile);
    }
}

void optionsHandleOutputFileArgument() {
    if (chosenOpts.verbose && chosenOpts.outputFile) {
        printf("[INFO -OPTIONS]: enviando salida (output) a %s\n", chosenOpts.outputFile);
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
    for (algorithm=faultHandlers; algorithm->name != NULL; algorithm++) {
        if (strcmp(algorithm->name, algorithmName) == 0) break;
    }

    if (algorithm->name == NULL) {
        fprintf(stderr, "[ERROR-OPTIONS]: No existe ningún algoritmo disponible con el nombre '%s'\n", algorithmName);
        exit(1);
    }
    else if (chosenOpts.verbose) {
        printf("[INFO -OPTIONS]: Usando algoritmo elegido '%s'\n", algorithmName);
    }

    chosenOpts.faultHandler = algorithm;
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
    printf("Usage: vmsim [OPTIONS] ALGORITHM [TRACEFILE|-]\n");
    printf("Process TRACEFILE, simulating a VM system. Reports stats on paging behavior.\n");
    printf("If TRACEFILE is not specified or is '-', input will be taken from stdin.\n");
    printf("\n");
    printf("ALGORITHM specifies the fault handler, and should be one of:\n");

    listAlgorithmsHelp();

    printf("\n");
    printf("Options:\n");
    printf("-h%-26s Print this message and exit.\n"                     , longOpt("|--help"));
    printf("-V%-26s Print the version information.\n"                   , longOpt("|--version"));
    printf("-v%-26s Verbose output. Includes progress output.\n"        , longOpt("|--verbose"));
    printf("-t%-26s Run self tests.\n"                                  , longOpt("|--test"));
    printf("-o FILE %-20s Append statistics output to the given file.\n", longOpt("|--output=FILE"));
    printf("-l REFS %-20s Stop after first REFS refs.\n"                , longOpt("|--limit=REFS"));
    printf("-p PAGES%-20s Simulate PAGES physical pages.\n"             , longOpt("|--pages=PAGES"));
    printf("%-28s Minimum value %d.\n"                                  , " ", MIN_PHYS_PAGES);
    printf("-s SIZE %-20s Simulate a page size of SIZE bytes.\n"        , longOpt("|--size=SIZE"));
    printf("%-28s Size must be a power of 2.\n"                         , " ");
}

char *longOpt(char *longoptHelp) {
    #ifdef HAVE_GETOPT_LONG
        return longoptHelp;
    #else
        return " ";
    #endif
}
/*
char *_longopt(char *longoptHelp) {
    #ifdef HAVE_GETOPT_LONG
        return longoptHelp;
    #else
        // duplicar la opción larga y cambiar cada caracter por un espacio en blanco
        char *spaces = strdup(longoptHelp);
        for (char *i=spaces; *i; i++) *i = ' ';
        return spaces;
    #endif
}
*/

char *showIfZlibAvailable() {
    return "";
    #ifdef HAVE_LIBZ
        return "Zlib is available; input may be compressed.\n";
    #else
        return "Zlib not available; input must be compressed.\nInstall Zlib (www.zlib.org) and rerun ./configure to add support.\n";
    #endif
}

void listAlgorithmsHelp() {
    fault_handler_info_t *algorithm;
    printf("   ");
    for (algorithm=faultHandlers; algorithm->name != NULL; algorithm++) {
        printf("%s%s", algorithm->name, (algorithm+1)->name ? ", " : "\n");
    }
}


