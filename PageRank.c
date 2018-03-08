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

       GET_TIME(start);
 
      // Initialize MPI
      MPI_Init(&argc, &argv);

      // Number of processes
      MPI_Comm_size(MPI_COMM_WORLD, &npes);
      
      // Get rank of current process
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

      // Test
      printf("Hello from process %d out of %d\n",my_rank, npes);
 
       // CORE CALCULATION
       do{
           // Count iterations
           ++iterationcount;
           // Copy rank as pre-existing rank
           vec_cp(r, r_pre, nodecount);
           
           // Loop over nodes
           for ( i = 0; i < nodecount; ++i){
               r[i] = 0;
               for ( j = 0; j < nodehead[i].num_in_links; ++j){
                   r[i] += r_pre[nodehead[i].inlinks[j]] / num_out_links[nodehead[i].inlinks[j]];
               }
               r[i] *= DAMPING_FACTOR;
               r[i] += damp_const;
           }
       }while(rel_error(r, r_pre, nodecount) >= EPSILON);

       MPI_Finalize();
      
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
