#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <omp.h>

#include "sort.h"
#include "edgelist.h"

// Order edges by id of a source vertex, 
// using the Counting Sort
// Complexity: O(E + V)
void countSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges, int digit_pos_) {

    
    int i;
    int key;
    int key_;
    int pos;

    // auxiliary arrays, allocated at the start up of the program
    int *vertex_cnt = (int*)malloc(numVertices*sizeof(int)); // needed for Counting Sort

      for(i = 0; i < 10; ++i) 
      {
        vertex_cnt[i] = 0;
      }
      
      // count occurrence of key: id of a source vertex      
      for(i = 0; i < numEdges; ++i) 
      {
        key = ((edges[i].src)/digit_pos_)%10;
        vertex_cnt[key]++;
      }
    // transform to cumulative sum
    for(i = 1; i < 10; ++i) {
        vertex_cnt[i] += vertex_cnt[i - 1];
    }
    

    // fill-in the sorted array of edges
    for(i = numEdges - 1; i >= 0; --i) {
        key_ = ((edges[i].src)/digit_pos_)%10;
        pos = vertex_cnt[key_] - 1;
        edges_sorted[pos] = edges[i];
        vertex_cnt[key_]--;
    }

    for(i = 0;i < numEdges; i++)
    {
    edges[i] = edges_sorted[i];
    } 

    free(vertex_cnt);

}


void radixSortEdgesBySource_parallel(struct Edge *edges_sorted, struct Edge *edges, int numOfVertices, int numOfEdges, int no_of_threads) 
{
  int max_value = numOfVertices;
  int digit_pos; 
  for(digit_pos = 1; max_value/digit_pos > 0; digit_pos *= 10)
  {
    countSortEdgesBySource_parallel(edges_sorted, edges, numOfVertices, numOfEdges, digit_pos, no_of_threads);
  }

}


void radixSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numOfVertices, int numOfEdges) 
{
  int max_value = numOfVertices;
  int digit_pos; 
  for(digit_pos = 1; max_value/digit_pos > 0; digit_pos *= 10)
  {
    countSortEdgesBySource(edges_sorted, edges, numOfVertices, numOfEdges, digit_pos);
  }

}



void countSortEdgesBySource_parallel(struct Edge *edges_sorted_, struct Edge *edges_, int numVertices, int numEdges, int digit_pos_, int no_of_threads) {

    int i;
    int j;
    int key;
    int key_;
    int pos_;
    int base = 0;
    // auxiliary arrays, allocated at the start up of the program
    int (*vertex_cnt_)[10] = malloc(sizeof(int[no_of_threads][10])); // needed for Counting Sort
    
    int edges_per_thread = numEdges/no_of_threads;
   
    #pragma omp parallel for private(i) num_threads(no_of_threads)
    for(j = 0; j < no_of_threads; j++)
    { 
      for(i = 0; i < 10; ++i) 
      {
        vertex_cnt_[j][i] = 0;
      }
    }

    // count occurrence of key: id of a source vertex
    #pragma omp parallel for private(i,key) num_threads(no_of_threads)
    for(j = 0; j < no_of_threads; j++)
    {
      if(j != no_of_threads - 1)
      {
        for(i = j*edges_per_thread; i < j*edges_per_thread + edges_per_thread; i++) 
        {
          key = ((edges_[i].src)/digit_pos_)%10;
          vertex_cnt_[j][key]++;
        }
      }
    
      else
      {
        for(i = j*edges_per_thread; i < numEdges; i++)
        {
          key = ((edges_[i].src)/digit_pos_)%10;
          vertex_cnt_[j][key]++;
        }
      }
    }

    // transform to cumulative sum
    // this loop cannot be parallelized, since it has dependencies.. 
    // making the loop iterations parallel here and including syncronization increased the overhead
    // hence this loop is not made to execute sequentially
    for(i = 0; i < 10; i++)
    {
      for(j = 0; j < no_of_threads; j++)
      {
        base = base + vertex_cnt_[j][i];
        vertex_cnt_[j][i] = base;
      }
    }
    

    // fill-in the sorted array of edges
    #pragma omp parallel for private(i,key_,pos_)
    for(j = 0; j < no_of_threads; j++)
    {
      if(j != no_of_threads - 1)
      {
        for(i = (j+1)*edges_per_thread - 1; i >= j*edges_per_thread; i--)
        {
          key_ = ((edges_[i].src)/digit_pos_)%10;
          pos_ = vertex_cnt_[j][key_] - 1;
          edges_sorted_[pos_] = edges_[i];
          vertex_cnt_[j][key_] = vertex_cnt_[j][key_] - 1; 
        }
      }
      else
      {
        for(i = numEdges - 1; i >= j*edges_per_thread; i--)
        {
          key_ = ((edges_[i].src)/digit_pos_)%10;
          pos_ = vertex_cnt_[j][key_] - 1;
          edges_sorted_[pos_] = edges_[i];
          vertex_cnt_[j][key_] = vertex_cnt_[j][key_] - 1; 
        }
      }
    }
    
    // updating the original array to be passed to next iteration of Radix sort
      #pragma omp parallel for shared(edges_, edges_sorted_) num_threads(no_of_threads)
      for(i = 0;i < numEdges; i++)
      {
        edges_[i] = edges_sorted_[i];
      } 
 
    free(vertex_cnt_);

}
