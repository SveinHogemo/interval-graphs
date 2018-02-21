#include <stdlib.h>
#include "graph_defs.h"

void edgelist_make_edge(struct edgelist *graph, int from, int to)
{
	if(graph->num_edges >= graph->size)
	{
		graph->size *= 2;
		graph->edges = realloc(graph->edges, graph->size*sizeof(int));
	}
	(graph->edges)[(graph->num_edges)++] = from;
	(graph->edges)[(graph->num_edges)++] = to;
}
