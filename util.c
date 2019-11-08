//
// Created by jhair on 5/11/19.
//

#include <assert.h>
#include <stdio.h>

#include "util.h"

uint log_2(uint argument) {
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

uint pow_2(uint exponent) {
    return 1 << exponent;
}

void utilTest() {
    printf("Testeando función log_2\n");
    assert(log_2(1UL) == 0);
    assert(log_2(2UL) == 1);
    assert(log_2(4UL) == 2);
    assert(log_2(1073741824UL) == 30);
    assert(log_2(2147483648UL) == 31);
    assert(log_2(3) == -1);
    assert(log_2(12) == -1);

    printf("Testeando función pow_2\n");
    assert(pow_2(0) == 1UL);
    assert(pow_2(1) == 2UL);
    assert(pow_2(2) == 4UL);
    assert(pow_2(3) == 8UL);
    assert(pow_2(30) == 1073741824UL);
    assert(pow_2(31) == 2147483648UL);
}
