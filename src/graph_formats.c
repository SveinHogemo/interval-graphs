#include <stdlib.h>
#include <stdio.h>
#include "graph_defs.h"

void write_dimacs_edgelist(struct edgelist *graph, char *filename)
{
	FILE *out = fopen(filename, "w");
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

struct edgelist * read_dimacs_edgelist(char *filename)
{
	struct edgelist *graph = malloc(sizeof(struct edgelist));
	int num_nodes, num_edges;

	FILE *in_file = fopen(filename, "r");
	int desc;
	int from, to;
	while((desc = fgetc(in_file)) >= 0)
	{
		switch(desc) {
		case 'c' :
			while(fgetc(in_file) != '\n');
			break;
		case 'p' :
			fscanf(in_file, " edge %d %d ", &num_nodes, &num_edges);
			num_edges *= 2;
			int *edges = malloc(num_edges*sizeof(int));
			*graph = (struct edgelist)
				{ num_nodes, edges, 0, num_edges };
			break;
		case 'e' :
			fscanf(in_file, "%d %d ", &from, &to);
			edgelist_make_edge(graph, from, to);
			break;
		default :
			fprintf(stderr, "Unknown descriptor in DIMACS file %s: %c\n", filename, desc);
		}
	}
	return graph;
}
