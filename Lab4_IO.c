#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"
/*----------------------------------------------------------------------------*/
int Lab4_saveoutput(double *R, int nodecount, double Time){
/*
    Save the data to file for Lab 4 

    -----
    Input:
    int *R         pointer to the result array 
    int nodecount  number of nodes 
    double Time    measured calculation time
    
    -----
    Output:
    data_output the stored data

    -----
    Example:
    lab4_saveoutput(R, nodecount, Time);
*/    
    FILE* op;
    int i;

    if ((op = fopen("data_output","w")) == NULL) {
        printf("Error opening the input file.\n");
        return 1;
    }
    fprintf(op, "%d\n%f\n", nodecount, Time);
    for (i = 0; i < nodecount; ++i )
        fprintf(op, "%e\n", R[i]);
    fclose(op);
    return 0;
}
#ifdef LAB4_EXTEND
/*----------------------------------------------------------------------------*/

int get_node_stat(int *nodecount, int **num_in_links, int **num_out_links){
/*
*  This function kinda loads your data. Gets the number of nodes, and how many
*  links there are coming to and leaving from a given node.
*
*   ip - File id to open and read data_input
*  src - Link from a given node
*  dst - Link to a given node
*
*  At the end it outputs:
*      nodecount - number of nodes used
*   num_in_links - number of links to a given node
*  num_out_links - number of links from a given node
*
*/
    FILE *ip;
    int i;
    int src, dst;
    
    // check if data_input exists
    if ((ip = fopen("data_input","r")) == NULL) {
        printf("Error opening the input file.\n");
        return 1;
    }

    // read nodecount from data_input
    fscanf(ip, "%d\n", nodecount);
    //fscanf(ip, "%d\n", &Ecount);
    
    /*
      Allocate memory for number of links in (to the node?)
      and out (to other node?).
    */ 
     (*num_in_links) = malloc((*nodecount) * sizeof(int)); 
    (*num_out_links) = malloc((*nodecount) * sizeof(int)); 
    
    // Loop over nodes and initialize num_in/out_links to zeros
    for (i = 0; i < (*nodecount); ++i){
        (*num_in_links)[i] = 0;
        (*num_out_links)[i] = 0;
    }
 
    while(!feof(ip)){
        /* 
           Count how many links there are leaving (src)
           and arriving (dst) for a given node
        */
        fscanf(ip, "%d\t%d\n", &src, &dst); 
        ++(*num_in_links)[dst];
        ++(*num_out_links)[src];
    } 

    fclose(ip);
    return 0;
}

/*----------------------------------------------------------------------------*/
int node_init(struct node **nodehead, int *num_in_links, int *num_out_links, int start, int end){
/*










*/
    FILE *ip;
    int i;
    int src, dst;
    int *index; //keep track of the outlink list storing position

    (*nodehead) = malloc((end - start) * sizeof(struct node));
    index = malloc((end - start) * sizeof(int));
    // // evaluate the extended in-link count
    // for ( i = 0; i < nodecount; ++i)
    //     if (num_out_links[i] == 0)
    //         ++non_out_count;
    // for ( i = 0; i < nodecount; ++i)
    //     num_in_links[i] += non_out_count;
    // create the nodes
    for ( i = start; i < end; ++i){
        (*nodehead)[i - start].num_in_links = num_in_links[i];
        (*nodehead)[i - start].num_out_links = num_out_links[i];
        (*nodehead)[i - start].inlinks = malloc((*nodehead)[i - start].num_in_links * sizeof(int));
        index[i - start] = 0;
    }
    if ((ip = fopen("data_input","r")) == NULL) {
        printf("Error opening the input file.\n");
        return 1;
    }
    fscanf(ip, "%d\n", &i);
    // normal case
    while(!feof(ip)){
        fscanf(ip, "%d\t%d\n", &src, &dst);
        if (dst >= start && dst < end)
            (*nodehead)[dst - start].inlinks[index[dst - start]++] = src;
    }
    // // deal with nodes with no outlinks, for those nodes, add a outlink for every node (including itself)
    
    // [Breno] Should we include that? 
    
    // for (i = 0; i < nodecount; ++i)
    //     if (num_out_links[i] == 0){
    //         num_out_links[i] = nodecount;
    //         (*nodehead)[i].num_out_links = nodecount;
    //         for (j = 0; j< nodecount; ++j)
    //             (*nodehead)[j].inlinks[index[j]++] = i;
    //     }
    free(index);
    fclose(ip);
    return 0;
}

/*----------------------------------------------------------------------------*/
int node_destroy(struct node *nodehead, int num_nodes){
/*
   Function to destroy nodes
*/
    int i;
    for (i = 0; i < num_nodes; ++i){
        free(nodehead[i].inlinks);
    }
    free(nodehead);
    return 0;
}

/*----------------------------------------------------------------------------*/
/*
   Function to compute the relative error between vectors r and t
*/
   double rel_error(double *r, double *t, int size){
    int i;
    double norm_diff = 0, norm_vec = 0;
    for (i = 0; i < size; ++i){
        norm_diff += (r[i] - t[i]) * (r[i] - t[i]);
        norm_vec += t[i] * t[i];
    }
    return sqrt(norm_diff)/sqrt(norm_vec);
}

/*----------------------------------------------------------------------------*/
/*
   Function to copy a vector into another
*/
int vec_cp(double *r, double *t, int size){
    int i;
    for (i = 0; i < size; ++i) t[i] = r[i];
    return 0;
}
#endif
