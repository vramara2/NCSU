#ifndef SORT_H
#define SORT_H

#include "edgelist.h"
// Order edges by id of a source vertex, 
// using the Counting Sort
// Complexity: O(E + V)
void countSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges, int digit_pos_);
void radixSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numOfVertices, int numOfEdges);
void countSortEdgesBySource_parallel(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges, int digit_pos_, int no_of_threads);
void radixSortEdgesBySource_parallel(struct Edge *edges_sorted, struct Edge *edges, int numOfVertices, int numOfEdges, int no_of_threads);
#endif
