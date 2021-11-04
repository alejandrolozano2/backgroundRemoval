#include "sorting.h"

/*Use count sorting method for edges (i,j,w)*/
void sortEdges( uint32_t (*es)[3], uint32_t (*ed)[3], uint32_t size, uint32_t maxVal ) { 
    #define WINDEX 2

    uint32_t  count[maxVal + 1] = {};
    uint32_t  index = 0;

    for ( int i = 0; i < size; i++ ) 
        count[es[i][WINDEX]] += 1;
    
    for ( int i = 1; i <= maxVal; i++ ) 
        count[i] += count[i-1];
    
    for ( int i = size -1 ; i >= 0; i--) {
        count[es[i][WINDEX]] -= 1;
        index = count[es[i][WINDEX]];
        ed[index][0] = es[i][0];
        ed[index][1] = es[i][1];
        ed[index][2] = es[i][2];
    }

}

/*Sort Count */

void countSort(uint32_t * source, uint32_t * destination, uint32_t size, uint32_t maxValue) {

    uint32_t  count[maxValue + 1] = {};

    for ( int i = 0; i < size; i++ ) 
        count[source[i]] += 1;
    

    for ( int i = 1; i <= maxValue; i++ ) 
        count[i] += count[i-1];
    
    for ( int i = size -1 ; i >= 0; i--) {
        count[source[i]] -= 1;
        destination[count[source[i]]] = source[i];
    }

}


void printArray(uint32_t * arr, uint32_t size) {

    for ( int  i = 0; i < size; i++) 
        std::cout << *arr++ << " ";
    
    std::cout << std::endl;
}

void printEdgeArray(uint32_t (*es)[3], uint32_t size) {

    for ( int  i = 0; i < size; i++) 
        std::cout << i << ": " << es[i][0] << " " << es[i][1] << " " << es[i][2] << " " << std::endl;
    
}



void printEdge(uint32_t ( & es)[3], uint32_t size) {

    std::cout << es[0] << " " << es[1] << " " << es[2] << " ";
    
    std::cout << std::endl;
}


void printBuffer(uint32_t * buffer, uint32_t size) {

    for ( int i = 0 ; i < size; i ++)
    std::cout << *buffer++ << " ";
    
    std::cout << std::endl;
}