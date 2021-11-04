#include "mst_image.h"

uint32_t findLeader(uint32_t * parents, uint32_t size, uint32_t value) {

    while(parents[value] != value) {
        value = parents[value];
    }

    return value;
}

bool join(uint32_t * parents, uint32_t * rank, uint32_t size, uint32_t i, uint32_t j) {

    uint32_t li = findLeader(parents, size, i);
    uint32_t lj = findLeader(parents, size, j);

    if (li == lj)
        return false;
    
    uint32_t ri = rank[li];
    uint32_t rj = rank[lj];

    if ((ri > rj) || (ri == rj && li > lj)) {
        parents[lj] = li;
        rank[li]+= rj;
    }
    else {
        parents[li]=lj;
        rank[lj]+= ri;
    }

    return true;
}

uint32_t kruskal(uint32_t  (* edges)[3], uint32_t eSize, uint32_t pSize, uint32_t * parents, uint32_t * ranks, uint32_t threshold) {
    uint32_t j, k, w;
    uint32_t mergedCount = 0;

    for (int i = 0 ; i < eSize; i++) {
        j = edges[i][0];
        k = edges[i][1];
        w = edges[i][2];

        if ( w > threshold )
            break;

        if ( join(parents, ranks, pSize, j ,k) == true)
            mergedCount++;

    }

    return mergedCount;
}