/*
    Test the result stored in "data_output" against a serial implementation.

    -----
    Compiling:
    Include "Lab4_IO.c" to compile. Set the macro "LAB4_EXTEND" defined in the "Lab4_IO.c" file to include the extended functions
    $ gcc serialtester.c Lab4_IO.c -o serialtester -lm 

    -----
    Return values:
    0      result is correct
    1      result is wrong
    2      problem size does not match
    253    no "data_output" file
    254    no "data_input" file
*/
#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

int main (int argc, char* argv[]){
    struct node *nodehead;
    int nodecount;
    int *num_in_links, *num_out_links;
    double *r, *r_pre;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    double start, end;
    int my_rank, npes;
    int local_a, local_b, local_node;
    

    // Load the data and simple verification
      
       // Gets the number of nodes, and how many links in/out from a given node.
       if (get_node_stat(&nodecount, &num_in_links, &num_out_links)){
           return 254;
       }

       // Calculate the result
       if (node_init(&nodehead, num_in_links, num_out_links, 0, nodecount)){
           return 254;
       } 

       // Allocate memory for serial rank and dummy rank_pre
       r = malloc(nodecount * sizeof(double));
       r_pre = malloc(nodecount * sizeof(double));
      
       // Initialize equal probabilities
       for ( i = 0; i < nodecount; ++i){
           r[i] = 1.0 / nodecount;
       }

       // Set up damping constant (1-d)/N in Eq (3).
       damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

      // Initialize MPI
      MPI_Init(&argc, &argv);

      // Number of processes
      MPI_Comm_size(MPI_COMM_WORLD, &npes);
      
      // Get rank of current process
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

      // Get locals
      local_node = nodecount/npes;
      local_a = my_rank*local_node;
      local_b = local_a + local_node;

      double* tmp = malloc(local_node * sizeof(double));

       // Timing
       GET_TIME(start);
  
       // CORE CALCULATION
       do{
           // Count iterations
           ++iterationcount;
           // Copy rank as pre-existing rank
           vec_cp(r, r_pre, nodecount);
           
           // Loop over nodes
           for ( i = 0; i<local_node; ++i){
               tmp[i] = 0;
               int indx = local_a + i;
               for ( j = 0; j < nodehead[indx].num_in_links; ++j){
                   tmp[i] += r_pre[nodehead[indx].inlinks[j]] / num_out_links[nodehead[indx].inlinks[j]];
               }
               tmp[i] *= DAMPING_FACTOR;
               tmp[i] += damp_const;
           }
           // Gather everything 
           MPI_Allgather(tmp, local_node, MPI_DOUBLE, r, local_node, MPI_DOUBLE, MPI_COMM_WORLD);
       }while(rel_error(r, r_pre, nodecount) >= EPSILON);

       free(tmp);

       MPI_Finalize();

       // Get final time
       GET_TIME(end);

      printf("Elapsed time %lf\n",end-start);

    // save output data
    Lab4_saveoutput(r,nodecount,end-start);

    //printf("Program converges at %d th iteration.\n", iterationcount);

    // post processing
    node_destroy(nodehead, nodecount);
    free(num_in_links); free(num_out_links);
    
    // terminate
    return 0;
}
