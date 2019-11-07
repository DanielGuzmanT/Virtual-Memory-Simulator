//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_FAULT_H
#define VMSIM_FAULT_H

#include <unistd.h>
#include "vmsim.h"
#include "pagetable.h"

typedef void (*fault_handler_t) (pte_t *pte, ref_kind_t type);

/* Fault Handlers son funciones que no retornan nada (void) y toman 2 argumentos:
 * - pte_t*     => es la nueva página que debe ser insertada
 * - ref_kind_t => es para el reporte estadístico
 * */
typedef struct _fault_handler_info {
    char *name;
    fault_handler_t  handler;
} fault_handler_info_t;

/* fault_handler_info_t nos permite relacionar la función actual del algoritmo
 * por su nombre. faultHandlers es iterado dentro de options.c para encontrar
 * el handler correcto de acuerdo al nombre ingresado por la línea de comandos. */
extern fault_handler_info_t faultHandlers[];

/* Inicializa cualquier estado necesario por este módulo */
void faultInit();

#endif //VMSIM_FAULT_H
