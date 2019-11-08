//
// Created by jhair on 5/11/19.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "pagetable.h"
#include "physmem.h"
#include "stats.h"

 pte_t **physicalMemory;

void physicalMemoryInit() {
    physicalMemory = (pte_t**)(calloc(chosenOpts.physicalPages, sizeof(pte_t*)));
    assert(physicalMemory);
}

pte_t **physicalMemoryArray() {
    return physicalMemory;
}

void physicalMemoryEvict(uint pfn, ref_kind_t type) {
    assert(0<=pfn && pfn<chosenOpts.physicalPages);

    // Ninguna página aquí - nada por hacer
    if (physicalMemory[pfn] == NULL || physicalMemory[pfn]->valid) {
        physicalMemory[pfn] = NULL;
        return;
    }

    #ifdef DEBUG
        printf("[DEBUG-PHYSMEM]: Desalojando marco de página con pfn=0x%x al disco\n", pfn);
//        printf("[DEBUG-PHYSMEM]: Desalojando marco de página con pfn=0x%x, type=%c al disco\n", pfn, type==REF_KIND_LOAD? 'R':'W');
    #endif

    statsEvict(type);
    if (physicalMemory[pfn]->modified) {
        statsEvictDirty(type);
    }

    physicalMemory[pfn]->pfn        = pfn;
    physicalMemory[pfn]->reference  = 0;
    physicalMemory[pfn]->modified   = 0;
    physicalMemory[pfn]->valid      = 1;
}

void physicalMemoryLoad(uint pfn, pte_t *newPage, ref_kind_t type) {
    assert(0<=pfn && pfn < chosenOpts.physicalPages);
    assert(newPage && !newPage->valid);
    assert(physicalMemory[pfn] == NULL);

    physicalMemory[pfn] = newPage;
    physicalMemory[pfn]->reference  = 0;
    physicalMemory[pfn]->modified   = 0;
    physicalMemory[pfn]->valid      = 1;
}

void physicalMemoryDump() {
//    printf("[INFO -PHYSMEM]: Campos de memoria física pfn:valid:reference:modified\n");
    printf("[INFO -PHYSMEM]: Campos de memoria física actual pfn:valid:reference:modified\n");
    for (int i=0; i<chosenOpts.physicalPages; i++) {
        pte_t *pte = (pte_t*)physicalMemory[i];
        if (pte) {
            printf("[INFO -PHYSMEM]: Memoria física[0x%x]:%d:0x%x:0x%x:%d:%d:%d\\n",
                    i,
                    pte->valid,
                    pte->vfn,
                    pte->pfn,
                    pte->modified,
                    pte->reference,
                    pte->counter);
        }
    }
}
