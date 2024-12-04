// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;
/* TODO: Define graph synchronization mechanisms. */
pthread_mutex_t mutex_process;


/* TODO: Define graph task argument. */

static void process_node(unsigned int idx)
{
	/* TODO: Implement thread-pool based processing of graph. */
	pthread_mutex_lock(&mutex_process);

	os_node_t *node;

	node = graph->nodes[idx];
	sum += node->info;
	graph->visited[idx] = DONE;

	for (unsigned int i = 0; i < node->num_neighbours; i++) {
		pthread_mutex_unlock(&mutex_process);
		if (graph->visited[node->neighbours[i]] == NOT_VISITED)
			process_node(node->neighbours[i]);
		pthread_mutex_lock(&mutex_process);
	}
	pthread_mutex_unlock(&mutex_process);
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	// initializare mutex
	pthread_mutex_init(&mutex_process, NULL);
	tp = create_threadpool(NUM_THREADS);
	process_node(0);
	wait_for_completion(tp);
	destroy_threadpool(tp);
	// de distrus si mutexurile
	pthread_mutex_destroy(&mutex_process);
	printf("%d", sum);

	return 0;
}
