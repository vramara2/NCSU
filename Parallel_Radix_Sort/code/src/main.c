#include <stdio.h>
#include <memory.h>

#include "bfs.h"
#include "sort.h"
#include "edgelist.h" 
#include "vertex.h"
#include "timer.h"

#define NUMBER_OF_THREADS 16

void printMessageWithtime(const char * msg, double time){

    printf(" -----------------------------------------------------\n");
    printf("| %-51s | \n", msg);
    printf(" -----------------------------------------------------\n");
    printf("| %-51f | \n", time);
    printf(" -----------------------------------------------------\n");

}


int main(void) {

    
    const char * fname = "./datasets/test/test.txt";
    //const char * fname = "./datasets/facebook/facebook_combined.txt";
    //const char * fname = "./datasets/wiki-vote/wiki-Vote.txt";
    //const char * fname = "./datasets/RMAT22";
    
    int numOfVertices = 0;
    int numOfEdges = 0;
    int no_of_threads = NUMBER_OF_THREADS;
    struct Timer* timer = (struct Timer*) malloc(sizeof(struct Timer));

    // get |v| |e| count do we can allocate our edge array and vertex array
    loadEdgeArrayInfo(fname, &numOfVertices, &numOfEdges);
    printf("Edges : %d Vertices: %d", numOfEdges, numOfVertices);

    // allocate our edge array and vertex array
    struct Edge * edgeArray = newEdgeArray(numOfEdges);
    struct Edge * edgeArray_parallel = newEdgeArray(numOfEdges);
    struct Edge * sortedEdgeArray = newEdgeArray(numOfEdges);
    struct Edge * sortedEdgeArray_parallel = newEdgeArray(numOfEdges);
    struct Vertex * vertexArray =  newVertexArray(numOfVertices);

    // populate the edge array from file
    loadEdgeArray(fname, edgeArray);
    loadEdgeArray(fname, edgeArray_parallel);
    
    //printEdgeArray(edgeArray, numOfEdges); // print the edge list unsorted 
    //printEdgeArray(edgeArray_parallel, numOfEdges); // print the edge list unsorted 

    /*the function you need to optimize*/
    Start(timer);
    radixSortEdgesBySource(sortedEdgeArray, edgeArray, numOfVertices, numOfEdges);
    //countSortEdgesBySource(sortedEdgeArray, edgeArray, numOfVertices, numOfEdges);
    Stop(timer);
    printMessageWithtime("\nTime Sorting (Seconds) for Radix Sort Serial",Seconds(timer));
    /*the function you need to optimize*/

    /*the function you need to optimize*/
    Start(timer);
    radixSortEdgesBySource_parallel(sortedEdgeArray_parallel, edgeArray_parallel, numOfVertices, numOfEdges, no_of_threads);
    Stop(timer);
    printMessageWithtime("\nTime Sorting (Seconds) Radix Sort Parallel",Seconds(timer));
    /*the function you need to optimize*/

    // if you want to check
    //printEdgeArray(sortedEdgeArray, numOfEdges); 
    //printEdgeArray(sortedEdgeArray_parallel, numOfEdges); 
    mapVertices(vertexArray, sortedEdgeArray_parallel, numOfVertices, numOfEdges);

    Start(timer);
    bfs(6, vertexArray, sortedEdgeArray_parallel, numOfVertices, numOfEdges);
    Stop(timer);
    printMessageWithtime("Time BFS (Seconds)",Seconds(timer));

    free(sortedEdgeArray);
    free(vertexArray);
    free(edgeArray);
    
    return 0;
}


