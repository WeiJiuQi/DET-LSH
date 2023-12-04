#ifndef index_support_h
#define index_support_h

#include "../../config.h"
#include "../../globals.h"
#include "utils/sax/ts.h"
#include "utils/sax/sax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_node.h"
#include "utils/isax/pqueue.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/isax/isax_node_split.h"


root_mask_type insert_to_index(isax_index *index, 
                                    sax_type * sax,
                                    file_position_type * pos,pthread_mutex_t *lock_firstnode,int workernumber,int total_workernumber);
void index_creation(long int data_point_num, isax_index *index);
void* index_creation_worker(void *transferdata);
enum response flush_subtree_leaf_buffers_inmemory (isax_index *index, isax_node *node);
isax_index * isax_index_init_inmemory(isax_index_settings *settings);

typedef struct buffer_data_inmemory
{
	isax_index *index;
	int start_number,stop_number;	
	data_type * data_point;
	pthread_mutex_t *lock_index;
	pthread_mutex_t *lock_firstnode;
	int workernumber;
	int total_workernumber;
	pthread_barrier_t *lock_barrier1;
	int *node_counter;
	bool finished;
	int *nodeid;
	unsigned long *shared_start_number;
	data_type ** lsh_hash_set;
}buffer_data_inmemory;

typedef struct transferfblinmemory
{
	int start_number,stop_number,conternumber;
  	int preworkernumber;
  	isax_index *index;
  	int *nodeid;
}transferfblinmemory;

data_type * rawfile;

typedef struct node_list
{
	isax_node **nlist;
	int node_amount;
}node_list;

isax_node * insert_to_pRecBuf(parallel_first_buffer_layer *fbl,
									sax_type *sax,
									file_position_type *pos,
									root_mask_type mask, 
									isax_index *index, 
									pthread_mutex_t *lock_firstnode,
									int workernumber,
									int total_workernumber);

#endif
