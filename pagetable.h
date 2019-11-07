//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_PAGETABLE_H
#define VMSIM_PAGETABLE_H

#include "vmsim.h"

/* valores por defecto que puedan ser sobrescritos desde la línea de comandos */
const static int pagesize = 4096;
const static log_pagesize = 12;

/* pte significa Page Table Entry, entrada de tabla de páginas */
typedef struct _pte {
    uint    vfn;
    uint    pfn;
    int     reference;
    bool_t  valid;
    bool_t  modified;
    int     counter;
} pte_t;

void pagetable_init();

/* Prestar atención a la Entrada de Tabla de Página (pte) para la página virtual dada.
 * Si la página no está en memoria, se seteará la variable 'valid'==0.
 * Si el Número de Frames Virtuales ('vfn') nunca ha sido visto antes, se creará un nuevo
 * pte_t con el 'vfn' dado y 'valid'==0.
 * variable 'type'
 * */
pte_t *pagetable_lookup_vaddr(uint vfn, ref_kind_t type);

void pagetable_test();

void pagetable_dump();

#endif //VMSIM_PAGETABLE_H
