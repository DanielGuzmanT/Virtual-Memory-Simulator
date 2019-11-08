//
// Created by jhair on 5/11/19.
//

#ifndef VMSIM_STATS_H
#define VMSIM_STATS_H

#include <sys/types.h>
#include <stdio.h>

#include "vmsim.h"

typedef uint count_t;
typedef count_t type_count_t[REF_KIND_NUM];

typedef struct _stats {
    type_count_t references;
    type_count_t miss;
    type_count_t compulsory;
    type_count_t evictions;
    type_count_t evictDirty;
    FILE *output;
} stats_t;

extern stats_t *stats;

void statsInit();
void statsOutput();

static inline void statsCompulsory(ref_kind_t type) {
    stats->compulsory[type]++;
}

static inline void statsReference(ref_kind_t type) {
    stats->references[type]++;
}

static inline void statsMiss(ref_kind_t type) {
    stats->miss[type]++;
}

static inline void statsEvict(ref_kind_t type) {
    stats->evictions[type]++;
}

static inline void statsEvictDirty(ref_kind_t type) {
    stats->evictDirty[type]++;
}

#endif //VMSIM_STATS_H
