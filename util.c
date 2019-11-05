//
// Created by jhair on 5/11/19.
//

#include <assert.h>
#include <stdio.h>

#include "util.h"

uint log2(uint argument) {
    int log = -1;

    for (int i=0; i<(8*sizeof(argument)); i++) {
        if (argument & (1 << i)) {
            // solo necesitamos potencias de 2 perfectas
            if (log != -1) return -1;
            log = i;
        }
    }

    return log;
}

uint pow2(uint exponent) {
    return 1 << exponent;
}

void utilTest() {
    printf("Testeando función log2\n");
    assert(log2(1UL) == 0);
    assert(log2(2UL) == 1);
    assert(log2(4UL) == 2);
    assert(log2(1073741824UL) == 30);
    assert(log2(2147483648UL) == 31);
    assert(log2(3) == -1);
    assert(log2(12) == -1);

    printf("Testeando función pow2\n");
    assert(pow2(0) == 1UL);
    assert(pow2(1) == 2UL);
    assert(pow2(2) == 4UL);
    assert(pow2(3) == 8UL);
    assert(pow2(30) == 1073741824UL);
    assert(pow2(31) == 2147483648UL);
}
