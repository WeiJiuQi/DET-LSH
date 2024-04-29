#ifndef query_support_h
#define query_support_h

#include "../../config.h"
#include "../../globals.h"
#include "utils/sax/ts.h"
#include "utils/sax/sax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_node.h"
#include "utils/isax/pqueue.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/isax/isax_node_split.h"
#include "indexing_support.h"

typedef struct query_result {
    float distance;
	float upper_distance;
	float lower_distance;
    isax_node *node;
    size_t pqueue_position;
} query_result;


typedef struct index_query_result {
    int number_of_index;
	query_result** result;
} index_query_result;


static int
cmp_pri(double next, double curr)
{
	return (next > curr);
}


static double
get_pri(void *a)
{
	return (double) ((query_result *) a)->lower_distance;
}


static void
set_pri(void *a, double pri)
{
	((query_result *) a)->lower_distance = (float)pri;
}


static size_t
get_pos(void *a)
{
	return ((query_result *) a)->pqueue_position;
}


static void
set_pos(void *a, size_t pos)
{
	((query_result *) a)->pqueue_position = pos;
}

typedef struct DETLSH_workerdata
{
	isax_node *current_root_node;
	data_type *lsh, *data_point;
	isax_index *index;
	pthread_mutex_t *lock_current_root_node;
	pthread_mutex_t *lock_queue;
	pthread_barrier_t *lock_barrier;
	pthread_rwlock_t *lock_bsf;
	query_result *bsf_result;
	int *node_counter;
	isax_node **nodelist;
	int amountnode;
	pthread_mutex_t *alllock;
	pqueue_t **allpq;
	float search_radius;
}DETLSH_workerdata;

void range_query(const char *ifilename, int q_num, isax_index *index, float search_radius,
                            pqueue_t ** (*search_function)(data_type*, data_type*, isax_index*,node_list*, float));
							
pqueue_t ** range_search_lsh (data_type *data_point, data_type *lsh, isax_index *index,node_list *nodelist,float search_radius);						
void* range_search_worker(void *workerdata);
void traverse_subtree(float *lsh,isax_node *node,isax_index *index,float search_radius,pqueue_t **pq,pthread_mutex_t *lock_queue,int *tnumber);

int N_PQUEUE;

int num_thread;

#endif
