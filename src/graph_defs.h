#ifndef GRAPH_DEFS_H
#define GRAPH_DEFS_H

struct edgelist { int num_nodes; int *edges; int num_edges; int size; };

void edgelist_make_edge(struct edgelist *, int, int);

#endif
