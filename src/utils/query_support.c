#ifdef VALUES
#include <values.h>
#endif
#include <float.h>
#include "../../config.h"
#include "../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#include "omp.h"  
#include "utils/indexing_support.h"
#include "utils/query_support.h"
#include "utils/indexing_support.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/isax/pqueue.h"
#include "utils/sax/sax.h"
#include "utils/encoding_support.h"
#include "utils/isax/isax_node_split.h"

#define NTHREutils 4

pqueue_t ** range_search_lsh (data_type *data_point, data_type *lsh, isax_index *index,node_list *nodelist,
                           float minimum_distance, int min_checked_leaves, float search_radius) 
{
    int node_counter=0;

    pqueue_t **allpq=malloc(sizeof(pqueue_t*)*N_PQUEUE);

    pthread_mutex_t ququelock[N_PQUEUE];
    int queuelabel[N_PQUEUE];

    isax_node *current_root_node = index->first_node;

    pthread_t threadid[maxquerythread];
    DETLSH_workerdata workerdata[maxquerythread];
    pthread_mutex_t lock_queue=PTHREAD_MUTEX_INITIALIZER,lock_current_root_node=PTHREAD_MUTEX_INITIALIZER;
    pthread_rwlock_t lock_bsf=PTHREAD_RWLOCK_INITIALIZER;
    pthread_barrier_t lock_barrier;
    pthread_barrier_init(&lock_barrier, NULL, maxquerythread);
 
    for (int i = 0; i < N_PQUEUE; i++)
    {
        allpq[i]=pqueue_init(index->settings->root_nodes_size/N_PQUEUE,
                        cmp_pri, get_pri, set_pri, get_pos, set_pos);
        pthread_mutex_init(&ququelock[i], NULL);
        queuelabel[i]=1;
    }

    for (int i = 0; i < maxquerythread; i++)
    {
        workerdata[i].lsh=lsh;
        workerdata[i].data_point=data_point;
        workerdata[i].lock_queue=&lock_queue;
        workerdata[i].lock_current_root_node=&lock_current_root_node;
        workerdata[i].lock_bsf=&lock_bsf;
        workerdata[i].nodelist=nodelist->nlist;
        workerdata[i].amountnode=nodelist->node_amount;
        workerdata[i].index=index;
        workerdata[i].minimum_distance=minimum_distance;
        workerdata[i].node_counter=&node_counter;
        workerdata[i].pq=allpq[i];
        workerdata[i].lock_barrier=&lock_barrier;
        workerdata[i].alllock=ququelock;
        workerdata[i].allqueuelabel=queuelabel;
        workerdata[i].allpq=allpq;
        workerdata[i].startqueuenumber=i%N_PQUEUE;
        workerdata[i].search_radius=search_radius;
    }  
    
    for (int i = 0; i < maxquerythread; i++)
    {
        pthread_create(&(threadid[i]),NULL,range_search_worker,(void*)&(workerdata[i]));
    }
    for (int i = 0; i < maxquerythread; i++)
    {
        pthread_join(threadid[i],NULL);
    }

    pthread_barrier_destroy(&lock_barrier);

    return allpq;

}

void* range_search_worker(void *rfdata)
{
    isax_node *current_root_node;
    query_result *n;
    isax_index *index=((DETLSH_workerdata*)rfdata)->index;
    data_type *lsh=((DETLSH_workerdata*)rfdata)->lsh;
    data_type *data_point=((DETLSH_workerdata*)rfdata)->data_point;
    pqueue_t *pq=((DETLSH_workerdata*)rfdata)->pq;
    query_result *do_not_remove = ((DETLSH_workerdata*)rfdata)->bsf_result;
    float minimum_distance=((DETLSH_workerdata*)rfdata)->minimum_distance;
    int limit=((DETLSH_workerdata*)rfdata)->limit;
    int checks = 0;
    bool finished=true;
    int current_root_node_number;
    int tight_bound = index->settings->tight_bound;
    int aggressive_check = index->settings->aggressive_check;
    query_result *bsf_result=(((DETLSH_workerdata*)rfdata)->bsf_result);
    float bsfdisntance=bsf_result->distance;
    int calculate_node=0,calculate_node_quque=0;
    int tnumber=rand()% N_PQUEUE;
    int startqueuenumber=((DETLSH_workerdata*)rfdata)->startqueuenumber;
    float search_radius=((DETLSH_workerdata*)rfdata)->search_radius;

    while (1) 
    {
        current_root_node_number=__sync_fetch_and_add(((DETLSH_workerdata*)rfdata)->node_counter,1);
        if(current_root_node_number>= ((DETLSH_workerdata*)rfdata)->amountnode)
        break;
        current_root_node=((DETLSH_workerdata*)rfdata)->nodelist[current_root_node_number];

        traverse_subtree(lsh,current_root_node,index,search_radius,((DETLSH_workerdata*)rfdata)->allpq,((DETLSH_workerdata*)rfdata)->alllock,&tnumber);
    }

    pthread_barrier_wait(((DETLSH_workerdata*)rfdata)->lock_barrier);
}

void traverse_subtree(float *lsh,isax_node *node,isax_index *index,float search_radius,pqueue_t **pq,pthread_mutex_t *lock_queue,int *tnumber)
{   
    float distance;

    query_result * dist_result = malloc(sizeof(query_result));

    query_to_node_dist(index, lsh, node->isax_values, node->isax_cardinalities, dist_result);
 
    if(dist_result->lower_distance < search_radius)
    {
        if (node->is_leaf) 
        {   
            dist_result->node = node;
            pthread_mutex_lock(&lock_queue[*tnumber]);
            pqueue_insert(pq[*tnumber], dist_result);
            pthread_mutex_unlock(&lock_queue[*tnumber]);
            *tnumber=(*tnumber+1)%N_PQUEUE;
            added_tree_node++;
        }
        else
        {   
            if (node->left_child->isax_cardinalities != NULL)
            {
                traverse_subtree(lsh,node->left_child,index, search_radius,pq,lock_queue,tnumber);
            }
            if (node->right_child->isax_cardinalities != NULL)
            {
                traverse_subtree(lsh,node->right_child,index,search_radius,pq,lock_queue,tnumber);
            }
        }
    }
}







