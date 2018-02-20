#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "graph_defs.h"
#include "graph_formats.h"

#define MAX 1023
#define INTERVAL_MAX 100

struct interval { double start; double end; };

int comp_starts(const void *first, const void *second) {
	double first_start = ((struct interval *)first)->start;
	double second_start = ((struct interval *)second)->start;
	return first_start-second_start;
}

void make_edge(struct edgelist *graph, int from, int to)
{
	if(graph->num_edges >= graph->size)
	{
		graph->size *= 2;
		graph->edges = realloc(graph->edges, graph->size*sizeof(int));
	}
	(graph->edges)[(graph->num_edges)++] = from;
	(graph->edges)[(graph->num_edges)++] = to;
}

struct edgelist * generate(int size)
{
	struct interval *intervals = malloc(size*sizeof(struct interval));
	srand48(time(0));
	for(int i = 0; i < size; i++)
	{
		double start = INTERVAL_MAX*drand48();
		double end = INTERVAL_MAX*drand48()+start;
		intervals[i] = (struct interval) { start, end };
	}
	qsort(intervals, size, sizeof(struct interval), comp_starts);
	struct edgelist *graph = malloc(sizeof(struct edgelist));
	int *edges = malloc(32*sizeof(int)); /* 32 is a nice, round number */
	*graph = (struct edgelist){ size, edges, 0, 32 };
	for(int i = 0; i < size; i++)
	{
		int j = i+1;
		double start = intervals[j].start;
		while(j < size && start < intervals[i].end)
		{
			make_edge(graph, i, j);
			start = intervals[++j].start;
		}
	}
	free(intervals);
	return graph;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		puts("\tUSAGE:\n");
		puts("generate-ig *output-file*");
		return 0;
	}
	srand(time(0));
	int size = MAX/(rand()%MAX)*3;
	struct edgelist *graph = generate(size);
	write_dimacs_edgelist(graph, argv[1]);
	return 0;
}

