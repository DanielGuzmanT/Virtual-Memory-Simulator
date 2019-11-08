//
// Created by jhair on 5/11/19.
//

// #include <config.h>
#include <stdlib.h>

#include "vmsim.h"
#include "physmem.h"
#include "stats.h"
#include "util.h"
#include "fault.h"
#include "options.h"

uint dotInterval = 100;
uint dotsPerLine = 64;

int refCounter = 0;

void init() {
    pagetableInit();
    physicalMemoryInit();
    statsInit();
    pagetableTest();
}

void test() {
    printf("[TEST -VMSIM]: Corriendo tests vmtrace...\n");
    utilTest();
    statsInit();
    pagetableTest();
}

/**************************************************************************/
ref_kind_t getType(char c) {
    if (c == 'R') return REF_KIND_LOAD ;
    if (c == 'W') return REF_KIND_STORE;
    return REF_KIND_CODE;
}

void simulate() {
    FILE *inputFile = NULL;

    #ifdef DEBUG
        char response[20]; uint pgFault = FALSE;
    #endif


    if ((inputFile=fopen(chosenOpts.inputFile, "r")) == NULL) {
        fprintf(stderr, "[ERROR-VMSIM]: No es posible abrir el archivo de entrada '%s'.\n", chosenOpts.inputFile);
        exit(1);
    }

    printf("\n");
    printf("[INFO -VMSIM]: Iniciando Simulación: \n");
    printf("[INFO -VMSIM]: Virtual Address tiene %d bits\n", addressSpaceBits);
    printf("[INFO -VMSIM]: %d bits más altos para VFN (Virtual Frame Number)\n", vfnBits);
    printf("[INFO -VMSIM]: %d bits más bajos para el offset dentro de cada página\n", log_2(chosenOpts.pageSize));
    printf("[INFO -VMSIM]: tamaño de la página => %d \n", chosenOpts.pageSize);

    fault_handler_t handler = chosenOpts.faultHandler->handler;
    uint pid; char ch; uint count = 0;
    vaddr_t virtualAdress;
    ref_kind_t type;
    pte_t *pte;

    while (fscanf(inputFile, "%d, %c, %x", &pid, &ch, &virtualAdress) != EOF) {
        type = getType(ch);
        statsReference(type);
        count++;

        if (chosenOpts.verbose && (count % dotInterval) == 0) {
            printf(".");
            fflush(stdout);

            if ((count % (dotsPerLine * dotInterval)) == 0) {
                printf("\n");
                fflush(stdout);
            }
        }

        pte = pagetableLookupVirtualAddress(virtualAddressToVFN(virtualAdress), type);

        /* FAULT */
        if (!pte->valid) {
            statsMiss(type);
            handler(pte, type);
        }

        pte->reference = 1;
        pte->counter = refCounter++; // usado por LRU
        if (type == REF_KIND_STORE) pte->modified = TRUE;

        if (chosenOpts.limit && count >= chosenOpts.limit) {
            if (chosenOpts.verbose) printf("[INFO -VMSIM]: %d referencias realizadas\n", count);
        }
    }


}

