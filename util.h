//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_UTIL_H
#define VMSIM_UTIL_H

#include <sys/types.h>
#include "vmsim.h"

#ifdef WORDS_BIGENDIAN
    /* Los archivos de rastreo se almacenan en little-endian.
     * Convertir si es necesario para este host. Tomar en cuenta
     * que la conversión de bytes individuales no es necesaria.
     * From http://remus.rutgers.edu/~rhoads/Code/swapbyte.c */
    static inline uint file_to_host_unit(uint tni4) {
        return ( ((tni4 >     24) &   0xFF) |
                 ((tni4 &   0xFF) <<    24) |
                 ((tni4 >>     8) & 0xFF00) |
                 ((tni4 & 0xFF00) <<     8)
                 );
    }

#else
    /* No requiere conversión */
    #define file_to_host_uint(i) (i)
#endif

/* De K&R, 2nd Ed., pg. 49: obtener los n bits desde la posición p */
static inline unit getBits(uint bitschain, int position, int nbits) {
    return bitschain >> (position + 1 - nbits) & ~(~0 << nbits);
}

static inline uint virtualAddressToVFN(vaddr_t virtualAddress) {
    return getBits(virtualAddress, addr_space_bits - 1, vfn_bits);
}

uint log2(uint number);
unit pow2(uint number);

void utilTest();

#endif //VMSIM_UTIL_H
