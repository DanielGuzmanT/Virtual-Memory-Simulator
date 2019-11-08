#include <stdio.h>
#include <stdlib.h>

#include "stats.h"
#include "options.h"
#include "vmsim.h"

int main(int argc, char **argv) {
    processOptions(argc, argv);
    if (chosenOpts.test) {
        test();
        printf("[INFO -MAIN]: Tests done\n");
        exit(0);
    }

    init();
    simulate();
    statsOutput();
    return 0;
}