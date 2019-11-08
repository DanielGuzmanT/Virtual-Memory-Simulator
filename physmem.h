//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_PHYSMEM_H
#define VMSIM_PHYSMEM_H

#include "vmsim.h"
#include "pagetable.h"

/* Función inicializadora de la memoria física a vacío */
void physicalMemoryInit();

/* Obtiene un array de pte_t's representando la memoria física.
 * No modificar este arreglo directamente. No modificar los elementos
 * de ésta directamente. USAR los elementos physicalMemoryEvict/physmem_load
 * en el array. Hay elementos chosenOpts.physPages en el array. Elementos
 * vacíos son NULL
 * */
pte_t **physicalMemoryArray();

/* Desalojar de la memoria la página obtenida del pfn dado. type debe
 * especificar el tipo de referencia que está causando el desalojo
 * (p.e. el tipo pasó el fault handler). Se marcará el pfn como vacío
 * (realizado por phymem_load)
 * */

void physicalMemoryEvict(uint pfn, ref_kind_t type);

/* Carga la página dada (pte) dentro del espacio de la memoria física
 * dada (pfn). El espacio debe estar vacío (ya sea porque nunca ha sido
 * usado, porque la página ha sido desalojada). 'type' debe especificar
 * qué tipo de referencia ha causó la carga
 * */
void physicalMemoryLoad(uint pfn, pte_t *newPage, ref_kind_t type);
void physicalMemoryDump();
extern pte_t **physicalMemory;

#endif //VMSIM_PHYSMEM_H
