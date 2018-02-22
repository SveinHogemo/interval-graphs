#include <stdlib.h>
#include <stdio.h>
#include "graph_formats.h"

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

void _adj_make_edge(struct adjlist *graph, int from, int to)
{
	if((graph->degrees)[from] >= (graph->sizes)[from])
	{
		(graph->sizes)[from] *= 2;
		(graph->edges)[from] = realloc(graph->edges,
			(graph->sizes)[from]*sizeof(int));
	}
	(graph->edges)[from][(graph->degrees)[from]++] = to;
}

void adjlist_make_edge(struct adjlist *graph, int from, int to)
{
	_adj_make_edge(graph, from, to);
	_adj_make_edge(graph, to, from);
}

/* non-destructive */
struct adjlist * edgelist_to_adjlist(struct edgelist *graph)
{
	struct adjlist *result = malloc(sizeof(struct adjlist));
	int num_nodes = graph->num_nodes;
	int average_degree = (graph->num_edges/(2*num_nodes))+1;
	int **edges = malloc(num_nodes*sizeof(int *));
	int *degrees = calloc(num_nodes, sizeof(int));
	int *sizes = malloc(num_nodes*sizeof(int));
	for(int i = 0; i < num_nodes; i++)
	{
		edges[i] = calloc(average_degree, sizeof(int));
		sizes[i] = average_degree;
	}
	*result = (struct adjlist) { num_nodes, edges, degrees, sizes };
	for(int i = 0; i < graph->num_edges; i += 2)
		adjlist_make_edge(result, (graph->edges)[i],
			(graph->edges)[i+1]);
	return result;
}

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
			fscanf(in_file, " edge %d %d ",
				&num_nodes, &num_edges);
			num_edges *= 2;
			int *edges = malloc(num_edges*sizeof(int));
			*graph = (struct edgelist)
				{ num_nodes, edges, 0, num_edges };
			break;
		case 'e' :
			fscanf(in_file, "%d %d ", &from, &to);
			edgelist_make_edge(graph, --from, --to);
			break;
		default :
			fprintf(stderr, "Unknown descriptor in DIMACS file %s: %c\n", filename, desc);
		}
	}
	return graph;
}
