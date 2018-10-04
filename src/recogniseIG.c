#include <stdlib.h>
#include <stdio.h>
#include "graph_formats.h"

struct nodeset { int *nodes; int crd; int size; };

struct nodeset_link
{
	struct nodeset *set;
	struct nodeset_link *previous;
	struct nodeset_link *next;
	struct nodeset_link *parent;
	int timestamp;
};

struct locate_pair { struct nodeset_link *link; int place; };

struct nodeset_locate { struct locate_pair **pairs; int size; };

struct nodeset_queue
{
	struct nodeset_link *first;
	struct nodeset_locate *locate;
	int length;
};

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

int nodeset_add_node(struct nodeset *set, int node)
{
	if(set->crd >= set->size)
	{
		set->size *= 2;
		set->nodes = realloc(set->nodes, set->size*sizeof(int));
	}
	set->nodes[set->crd++] = node;
	return set->crd-1;
}

int nodeset_delete_node(struct nodeset *set, int where)
{
	set->nodes[where] = set->nodes[--set->crd];
	return set->nodes[where];
}

void delete_link(struct nodeset_link *link) {
	delete_nodeset(link->set);
	free(link);
}

void delete_locate(struct nodeset_locate *locate)
{
	for(int i = 0; i < locate->size; i++)
		free(locate->pairs[i]);
	free(locate->pairs);
	free(locate);
}

struct nodeset_queue * make_nodeset_queue(int num_nodes, int timestamp)
{
	struct nodeset *set = make_nodeset(num_nodes);
	for(set->crd = 0; set->crd < num_nodes; set->crd++)
		set->nodes[set->crd] = set->crd;
	struct nodeset_link *link = malloc(sizeof(struct nodeset_link));
	*link = (struct nodeset_link) { set, 0, 0, 0, timestamp };
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
	*queue = (struct nodeset_queue) { link, locate, 1 };
	return queue;
}

void delete_nodeset_queue(struct nodeset_queue *queue)
{
	struct nodeset_link *link = queue->first;
	while(link)
	{
		link = link->next;
		delete_link(queue->first);
		queue->first = link;
	}
	delete_locate(queue->locate);
	free(queue);
}

void dequeue(struct nodeset_queue *queue)
{
	struct nodeset_link *next = queue->first->next;
	delete_link(queue->first);
	queue->first = next;
	if(next)
		next->previous = 0;
	queue->length--;
}

int pick(struct nodeset_queue *queue)
{
	struct nodeset *first = queue->first->set;
	int chosen = first->nodes[--first->crd];
	if(!first->crd)
		dequeue(queue);
	struct locate_pair *located = queue->locate->pairs[chosen];
	located->link = 0;
	located->place = -1;
	return chosen;
}

void split_queue(struct nodeset_queue *queue, int pivot, int timestamp)
{
	struct locate_pair *is_located = queue->locate->pairs[pivot];
	struct nodeset_link *link = is_located->link;
	struct nodeset_link *previous = link->previous;
	if(!previous || previous->timestamp < timestamp ||
		previous->parent != link)
	{
		struct nodeset *new_set = make_nodeset(link->set->size/2+1);
		struct nodeset_link *new_link = malloc
			(sizeof(struct nodeset_link));
		*new_link = (struct nodeset_link)
			{ new_set, previous, link, link, timestamp };
		if(!previous)
			link->previous = queue->first = new_link;
		else
			link->previous = previous->next = new_link;
		previous = new_link;
		queue->length++;
	}
	int displaced = nodeset_delete_node(link->set, is_located->place);
	queue->locate->pairs[displaced]->place = is_located->place;
	if(link->set->crd == 0)
	{
		if(link->next)
			link->next->previous = previous;
		previous->next = link->next;
		delete_link(link);
		queue->length--;
	}
	int where = nodeset_add_node(previous->set, pivot);
	struct locate_pair *located = queue->locate->pairs[pivot];
	located->link = previous;
	located->place = where;
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
		{
			int target = graph->edges[node][i];
			if(queue->locate->pairs[target]->link)
				split_queue(queue, target, current_rank);
		}
		current_rank++;
	}
	while(queue->first)
	{
		ordering[current_rank++] = pick(queue);
	}
	delete_nodeset_queue(queue);
	return ordering;
}

int check_chordal(struct adjlist *graph, int *ordering)
{
	struct nodeset **bucket_to = malloc
		(graph->num_nodes*sizeof(struct nodeset *));
	struct nodeset **bucket_from = malloc
		(graph->num_nodes*sizeof(struct nodeset *));
	for(int i = 0; i < graph->num_nodes; i++)
		bucket_to[i] = make_nodeset(2);
		bucket_from[i] = make_nodeset(2);
	for(int i = 0; i < graph->num_nodes; i++)
		for(int j = 0; j < graph->degrees[i]; j++) {
			int k = graph->edges[i][j];
			if(ordering[i] < ordering[k])
				nodeset_add_node(bucket_to[k], i);
		}
	for(int i = 0; i < graph->num_nodes; i++)
		for(int j = 0; j < bucket_to[i]->crd, j++)
			int k = bucket_to[i]->nodes[j];
			nodeset_add_node(bucket_from[k], i);
	for(int i = 0; i < graph->num_nodes; i++)
		if(bucket_from[i]->crd) {
			int parent = bucket_from[i]->
				nodes[bucket_from[i]->crd-1];
			int n_j = 0;
			int p_j = 0;
			while(n_j < bucket_from[i]->crd-1) {
				int k = bucket_from[i]->nodes[n_j];
				if(k == parent) {
					n_j++;
					continue;
				}
				while(ordering[k] > ordering
					[bucket_from[parent]->nodes[p_j]] &&
					p_j < bucket_from[parent]->crd)
					p_j++;
				if(p_j >= bucket_from[parent]->crd ||
					ordering[k] < ordering
					[bucket_from[parent]->nodes[p_j]])
					return 0;
				n_j++;
			}
		}
	return 1;
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
	/* delete_edgelist(edges); */
	int *ordering = lex_bfs(graph);
	printf("The ordering of nodes according to Lex-BFS is:");
	for(int i = 0; i < graph->num_nodes; i++)
		printf(" %d", ordering[i]+1);
	printf("\n");
/*	if(!check_chordal(graph, ordering))
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

