#ifndef SORTING_H
#define SORTING_H

#include <iostream>
#include <time.h>
#include <cstdlib>

void sortEdges( uint32_t (* es)[3], uint32_t (* ed)[3], uint32_t size , uint32_t maxVal);
void countSort(uint32_t * source, uint32_t * destination, uint32_t size, uint32_t maxValue);
void printArray(uint32_t * arr, uint32_t size);
void printEdge(uint32_t ( & es)[3], uint32_t size);
void printEdgeArray(uint32_t (*es)[3], uint32_t size);
void printBuffer(uint32_t * buffer, uint32_t size);

#endif