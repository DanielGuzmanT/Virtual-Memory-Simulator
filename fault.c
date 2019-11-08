//
// Created by jhair on 5/11/19.
//

#include "vmsim.h"
#include "fault.h"
#include "options.h"
#include "physmem.h"
#include <stdlib.h>
#include <stdio.h>


/* algoritmos implementados hasta el momento */
void faultRandom(pte_t *pte, ref_kind_t type);
void faultClock (pte_t *pte, ref_kind_t type);
void faultFifo  (pte_t *pte, ref_kind_t type);
void faultLru   (pte_t *pte, ref_kind_t type);


fault_handler_info_t faultHandlers[6] = {
        {"random", faultRandom  },
        {"fifo"  , faultClock   },
        {"clock" , faultFifo    },
        {"lru"   , faultLru     },
        {NULL    , NULL }, // última entrada debe ser siempre NULL/NULL
};

// TODO(7): eliminar esta función si no llega a ser utilizada
void faultInit() {
    // does nothing
}

/* ==================================================================== */
/* =================== IMPLEMENTACIÓN DE ALGORITMOS =================== */

void faultRandom(pte_t *pte, ref_kind_t type) {
    int frame = (int)(random() % chosenOpts.physicalPages);
    physicalMemoryEvict(frame, type);
    physicalMemoryLoad(frame, pte, type);
}

void faultFifo(pte_t *pte, ref_kind_t type) {
    printf("FIFO aún no implementado!\n");
}

void faultClock(pte_t *pte, ref_kind_t type) {
    printf("CLOCK aún no implementado!\n");
}

void faultLru(pte_t *pte, ref_kind_t type) {
    printf("LRU aún no implementado!\n");
}

/* ==================================================================== */
