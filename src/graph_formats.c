#include <stdlib.h>
#include <stdio.h>
#include "graph_defs.h"

void write_dimacs_edgelist(struct edgelist *graph, char *filename)
{
	FILE *out = fopen(filename, "a");
	fprintf(out, "c File: %s\n", filename);
	fprintf(out, "c This is a randomly generated\n");
	fprintf(out, "c interval graph\n");
	fprintf(out, "p edge %d %d\n", graph->num_nodes, graph->num_edges/2);
	for(int i = 0; i < graph->num_edges; i++)
	{
		fprintf(out, "e %d", (graph->edges)[i]+1);
		i++;
		fprintf(out, " %d\n", (graph->edges)[i]+1);
	}
	fclose(out);
}
