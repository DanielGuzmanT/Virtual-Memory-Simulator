//
// Created by jhair on 5/11/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "stats.h"
#include "options.h"

stats_t* stats;

void statsOutputType(FILE *output, type_count_t outputType, const char *label);

void statsInit() {
    stats = (stats_t*)calloc(1, sizeof(stats_t));
    assert(stats);

    if (chosenOpts.outputFile) {
        stats->output = fopen(chosenOpts.outputFile, "a+");

        if (stats->output == NULL) {
            perror("[ERROR-STATS]: No es posible abrir el archivo de salir para escribir\n");
            abort();
        }

        else {
            stats->output = stdout;
        }
    }
}

void statsOutput() {
    FILE *output = stats->output;
    fprintf(output, "\n\n[INFO -STATS]: PARÁMETROS DE LA SIMULACIÓN:\n\n");
    fprintf(output, "physicalPages  => %d \n", chosenOpts.physicalPages);
    fprintf(output, "pageSize       => %d \n", chosenOpts.pageSize);
    fprintf(output, "inputFile      => %s \n", chosenOpts.inputFile ? chosenOpts.inputFile : "stdin");
    fprintf(output, "faultHandler   => %s \n", chosenOpts.faultHandler->name);
    fprintf(output, "referenceLimit => %ld\n", chosenOpts.limit);

    fprintf(output, "\n\n[INFO -STATS]: RESULTADOS DE LA SIMULACIÓN:\n\n");

    fprintf(output, "\t%25s | %10s | %10s | %10s | %10s\n", "Stat Type","code", "load", "store", "total");
    statsOutputType(output, stats->references, "Memory references");
    statsOutputType(output, stats->miss      , "Page Faults");
    statsOutputType(output, stats->compulsory, "Compulsory Page Faults");
    statsOutputType(output, stats->evictDirty, "(Dirty) Page Writes");

    fclose(output);
    stats->output = NULL;
}

void statsOutputType(FILE* output, type_count_t outputType, const char *label) {
    fprintf(output, "\t%25s | %10u | %10u | %10u | %10u\n", label,
            outputType[REF_KIND_CODE ],
            outputType[REF_KIND_LOAD ],
            outputType[REF_KIND_STORE],
            outputType[REF_KIND_CODE ] +
            outputType[REF_KIND_LOAD ] +
            outputType[REF_KIND_STORE]);
}
