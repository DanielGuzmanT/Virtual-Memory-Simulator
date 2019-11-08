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

// TODO(2): ¿Qué significan los tipos de variables vaddr_t y paddr_t?
typedef uint vaddr_t;
typedef uint paddr_t;

typedef enum _ref_kind {
    REF_KIND_CODE  = 0,
    REF_KIND_LOAD  = 1,
    REF_KIND_STORE = 2
} ref_kind_t;

// TODO(3): ¿Por qué REF_KIND_NUM no está dentro del enum ref_kind_t?
#define REF_KIND_NUM  3

/* tamaño del espacio de direcciones */
const static uint addressSpaceBits = 16;
// TODO(4): ¿Qué significa vfnBits?, ¿virtual frame bits number?
extern uint vfnBits;
// TODO(5): ¿En dónde se usa ref_counter?
extern int ref_counter;

#endif //VMSIM_VMSIM_H
