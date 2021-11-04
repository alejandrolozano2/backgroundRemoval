#ifndef MST_IMAGE_H
#define MST_IMAGE_H

#include<opencv2/core/core.hpp>
#include<opencv2/ml/ml.hpp>
#include<opencv/cv.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>
#include <time.h>
#include <cstdlib>


/*Finds parent. Used for find/joing algorithm*/
uint32_t findLeader(uint32_t * parents, uint32_t size, uint32_t value);

/*Join Elements*/
bool join(uint32_t * parents, uint32_t * rank, uint32_t size, uint32_t i, uint32_t j);

/*MST Kruskal i,j,w*/
uint32_t kruskal(uint32_t  (* edges)[3], uint32_t eSize, uint32_t pSize, uint32_t * parents, uint32_t * ranks, uint32_t threshold);


static inline  __attribute__((always_inline)) void initParentsAndRanks(uint32_t * parents, uint32_t * ranks, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        *parents++ = i;
        *ranks++ = 1;
    }
}
static inline __attribute__((always_inline)) void initRanks(uint32_t * ranks, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        *ranks++ = 1;
    }
}

static inline __attribute__((always_inline)) uint32_t getMaxParent(uint32_t * ranks, uint32_t len) {
    
    uint32_t max = 0;
    for (uint32_t i = 0; i < len; i++) {
        if ( ranks[i] > ranks[max] ) max = i;
    }

    return max;
}

#endif