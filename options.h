//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_OPTIONS_H
#define VMSIM_OPTIONS_H

#include "vmsim.h"
#include "fault.h"

typedef struct _opts {
    bool_t verbose;
    bool_t test;
    int pageSize;
    int physicalPages;
    long limit;
    char *outputFile;
    char *inputFile;
    fault_handler_info_t *faultHandler;
} opts_t;

extern opts_t chosenOpts;

void processOptions(int argc, char **argv);

#endif //VMSIM_OPTIONS_H
