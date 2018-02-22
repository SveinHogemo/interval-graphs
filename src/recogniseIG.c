#include <stdlib.h>
#include <stdio.h>
#include "graph_defs.h"
#include "graph_formats.h"

struct nodeset { int *nodes; int cardinality; int size; };

struct nodeset * make_nodeset(int size)
{
	int *nodes = calloc(size, sizeof(int));
	struct nodeset *set = malloc(sizeof(struct nodeset));
	*set = (struct nodeset) { nodes, 0, size };
	return set;
}

void delete_nodeset(struct nodeset *set)
{
	free(set->nodes);
	free(set);
}

void nodeset_add_node(struct nodeset *set, int node)
{
	if(set->cardinality >= set->size)
	{
		size *= 2;
		set->nodes = realloc(size*sizeof(int));
	}
	set->nodes[set->cardinality++] = node;
}

void nodeset_delete_node(struct nodeset *set, int where)
{
	set->nodes[where] = set->nodes[--set->cardinality];
}

struct nodeset_link
{
	struct nodeset *set;
	struct nodeset_link *previous;
	struct nodeset_link *next;
	int timestamp;
};

void delete_link(struct nodeset_link *link) {
	delete_nodeset(link->set);
	free(link);
}

struct locate_pair { struct nodeset_link *link, int place };

struct nodeset_locate { struct locate_pair **pairs; int size; };

struct nodeset_queue
{
	struct nodeset_link *first;
	struct nodeset_locate *locate;
};

struct nodeset_queue * make_nodeset_queue(int num_nodes, int timestamp)
{
	struct nodeset *set = make_nodeset(num_nodes);
	for(set->cardinality = 0; set->cardinality < num_nodes; set->cardinality++)
		nodes[set->cardinality] = set->cardinality;
	struct nodeset_link *link = malloc(sizeof(struct nodeset_link));
	*link = (struct nodeset_link) { set, 0, 0, timestamp };
	struct locate_pair **pairs = malloc
		(num_nodes*sizeof(struct locate_pair *));
	for(int i = 0; i < num_nodes; i++)
	{
		pairs[i] = malloc(sizeof(struct locate_pair));
		*(pairs[i]) = (struct locate_pair) { link, i };
	}
	struct nodeset_locate *locate = malloc(sizeof(struct nodeset_locate));
	*locate = (struct nodeset_locate) { pairs, num_nodes };
	struct nodeset_queue *queue = malloc(sizeof(struct nodeset_queue));
	*queue = (struct nodeset_queue) { link, locate };
}

void dequeue(struct nodeset_queue *queue)
{
	struct nodeset_link *next = queue->first->next;
	delete_link(queue->first);
	queue->first = next;
}

int pick(struct nodeset_queue *queue)
{
	struct nodeset *first = queue->first->set;
	int chosen = set->nodes[--set->cardinality];
	if(!set->cardinality)
		dequeue(queue);
	return chosen;
}

void split_queue(struct nodeset_queue *queue, int pivot, int timestamp)
{
	struct locate_pair *is_located = queue->locate->pairs[pivot];
	struct nodeset_link *link = locate_pair->link;
	struct nodeset_link *previous = link->previous;
	if(is_located->link->timestamp < timestamp)
	{
		struct nodeset *new_set = make_nodeset(link->set->size/2);
		struct nodeset_link *new_link = malloc
			(sizeof(struct nodeset_link));
		*new_link = (struct nodeset_link)
			{ set, previous, link, timestamp };
		link->previous = previous->next = new_link;
		previous = new_link;
	}
	nodeset_delete_node(link->set, is_located->place);
	nodeset_add_node(previous->set, pivot);
}

int * lex_bfs(struct adjlist *graph)
{
	int current_rank = 0;
	struct nodeset_queue *queue = make_nodeset_queue
		(graph->num_nodes, current_rank);
	int *ordering = malloc(graph->num_nodes*sizeof(int));
	while(queue->length < graph->num_nodes - current_rank)
	{
		int node = pick(queue);
		ordering[current_rank] = node;
		for(int i = 0; i < graph->degrees[node]; i++)
			split_queue(queue, graph->edges[node][i],
				current_rank);
		current_rank++;
	}
	while(queue)
		ordering[current_rank++] = pick(queue);
	delete_queue(queue);
	return ordering;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		puts("Use: recognise-ig *input-file*");
		return 0;
	}
	struct edgelist *edges = read_dimacs_edgelist(argv[1]);
	struct adjlist *graph = edgelist_to_adjlist(edges);
	delete_edgelist(edges);
	int *ordering = lex_bfs(graph);
	printf("The ordering of nodes according to Lex-BFS is:");
/*	for(int i = 0; i < graph->num_nodes; i++)
		printf(" %d", ordering[i]+1);
	printf("\n");
	if(!check_chordal(graph, ordering))
	{
		puts("Is not chordal graph or interval graph");
		return 0;
	}
	puts("Is chordal graph");
	struct clique_tree *cliques = make_clique_tree(graph, ordering);
	if(check_interval(graph, ordering, cliques)
		puts("Is interval graph");
	else
		puts("Is not interval graph"); */
	return 0;
}
