//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_PAGETABLE_H
#define VMSIM_PAGETABLE_H

#include "vmsim.h"

/* valores por defecto que puedan ser sobrescritos desde la línea de comandos */
const static int pagesize = 4096;
const static int logPagesize = 12;

/* pte significa Page Table Entry, entrada de tabla de páginas 
 * https://www.geeksforgeeks.org/page-table-entries-in-page-table/
 * */
typedef struct _pte {
    uint    vfn;        // el número del frame virtual (Virtual Frame Number)
    uint    pfn;        // el número del frame físico (Physical Frame Number) if valid=1
    int     reference;  // si ha sido accedidad en el último ciclo de reloj o no. 1 si la pág. es accedida
    bool_t  valid;      // Verdadero si esta en la memoria física, de otra manera Falso (fault page)
    bool_t  modified;   // Limpio o sucio. Se vuelve a guardar con los cambios hechos si está sucia-modificada (evita escribir en intercambio a disco duro)
    int     counter;    // usado por LRU, FIFO
} pte_t;

void pagetableInit();

/* Prestar atención a la Entrada de Tabla de Página (pte) para la página virtual dada.
 * Si la página no está en memoria, se seteará la variable 'valid'==0.
 * Si el Número de Frames Virtuales ('vfn') nunca ha sido visto antes, se creará un nuevo
 * pte_t con el 'vfn' dado y 'valid'==0.
 * variable 'type'
 * */
pte_t *pagetableLookupVirtualAddress(uint vfn, ref_kind_t type);

void pagetableTest();

void pagetableDump();

#endif //VMSIM_PAGETABLE_H
