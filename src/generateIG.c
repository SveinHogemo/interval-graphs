#include <stdlib.h>
#include <stdio.h>

#define MAX 1024
#define INTERVAL_MAX 100

struct interval { double start; double end; };

struct graph { int num_nodes; int *edges; int size; int length; };

int comp_starts(const void *first, const void *second) {
	double first_start = ((struct interval *)first)->start;
	double second_start = ((struct interval *)second)->start;
	return second_start-first_start;
}

void make_edge(struct graph *graph, int from, int to)
{
	if(graph->size >= graph->length)
	{
		graph->length *= 2;
		graph->edges = realloc(graph->edges, graph->length);
	}
	(graph->edges)[(graph->size)++] = from;
	(graph->edges)[(graph->size)++] = to;
}

struct graph * generate(int size)
{
	struct interval *intervals = malloc(size*sizeof(struct interval));
	for(int i = 0; i < size; i++)
	{
		double start = INTERVAL_MAX*drand48();
		double end = INTERVAL_MAX*drand48()+start;
		intervals[i] = (struct interval) { start, end };
	}
	qsort(intervals, size, sizeof(struct interval), comp_starts);
	struct graph *graph = malloc(sizeof(struct graph));
	int *edges = malloc(32*sizeof(int)); /* 32 is a nice, round number */
	*graph = (struct graph){ size, edges, 0, 32 };
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
