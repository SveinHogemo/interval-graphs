#ifndef GRAPH_FORMATS_H
#define GRAPH_FORMATS_H

/* Simply stores node numbers in an array.
   Two consecutive nodes form an edge.
   num_edges is therefore two times as the number of edges.
   size is the size of the edges array. */
struct edgelist { int num_nodes; int *edges; int num_edges; int size; };

struct adjlist { int num_nodes; int **edges; int *degrees; int *sizes; };

void edgelist_make_edge(struct edgelist *, int, int);

void write_dimacs_edgelist(struct edgelist *, char *);

struct edgelist * read_dimacs_edgelist(char *);

#endif
