//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_VMSIM_H
#define VMSIM_VMSIM_H

#include <sys/types.h>

#define FALSE   0
#define TRUE    1

typedef int bool_t;
typedef unsigned char byte_t;

typedef uint vaddr_t;   // virtual address type, cada de bits de un lugar en la memoria virtual
typedef uint paddr_t;   // pysical address type, cada de bits de un lugar en la memoria física

typedef enum _ref_kind {
    REF_KIND_CODE  = 0,
    REF_KIND_LOAD  = 1,
    REF_KIND_STORE = 2
} ref_kind_t;

#define REF_KIND_NUM  3

/* tamaño del espacio de direcciones */
const static uint addressSpaceBits = 16;
extern uint vfnBits;
extern int ref_counter;

#endif //VMSIM_VMSIM_H
