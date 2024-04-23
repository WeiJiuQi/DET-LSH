#ifdef VALUES
#include <values.h>
#endif
#include <float.h>
#include "../../config.h"
#include "../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include "utils/indexing_support.h"
#include "utils/query_support.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/isax/pqueue.h"
#include "utils/sax/sax.h"
#include "utils/isax/isax_node_split.h"
#include "utils/encoding_support.h"
#include "utils/isax/isax_node.h"
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_node_record.h"

void index_creation(long int data_point_num, isax_index *index)
{

    index->sax_file=NULL;

    long int data_point_loaded = 0;
    int i;
    int node_counter=0;
    pthread_t threadid[num_thread];
    buffer_data_inmemory *input_data=malloc(sizeof(buffer_data_inmemory)*(num_thread));
    // index->sax_cache= malloc(sizeof(sax_type) * index->settings->LSH_dimensionality*data_point_num);
    // index->lsh_cache= malloc(sizeof(data_type) * index->settings->LSH_dimensionality*data_point_num);
    pthread_barrier_t lock_barrier1;
    pthread_barrier_init(&lock_barrier1, NULL, num_thread);

    pthread_mutex_t lock_firstnode=PTHREAD_MUTEX_INITIALIZER;
    
    destroy_fbl(index->fbl);
    index->fbl = (first_buffer_layer*)initialize_pRecBuf(index->settings->initial_fbl_buffer_size,
                            pow(2, index->settings->LSH_dimensionality), 
                            index->settings->max_total_buffer_size+DISK_BUFFER_SIZE*(PROGRESS_CALCULATE_THREAD_NUMBER-1), index);
    
    for ( i = 0; i < num_thread; i++)
    {   
        input_data[i].index=index;
        input_data[i].lock_firstnode =&lock_firstnode;
        input_data[i].data_point=rawfile;
        input_data[i].workernumber=i;
        input_data[i].total_workernumber=num_thread;
        input_data[i].start_number=i*(data_point_num/num_thread);
        input_data[i].stop_number=(i+1)*(data_point_num/num_thread);
        input_data[i].node_counter=&node_counter;
        input_data[i].lock_barrier1=&lock_barrier1;
    }

    input_data[num_thread-1].start_number=(num_thread-1)*(data_point_num/num_thread);
    input_data[num_thread-1].stop_number=data_point_num;
    for (i = 0; i < num_thread; i++)
    {
        pthread_create(&(threadid[i]),NULL,index_creation_worker,(void*)&(input_data[i]));
    }

    for (i = 0; i < num_thread; i++)
    {
        pthread_join(threadid[i],NULL);
    }
    __sync_fetch_and_add(&(index->total_records),data_point_num);
    index->sax_cache_size=index->total_records;
    free(input_data);

    destroy_pRecBufdet((parallel_first_buffer_layer*)(index->fbl),num_thread);
    fprintf(stderr, ">>> Finished encoding and indexing.\n");
}


void* index_creation_worker(void *transferdata)
{
	unsigned long int transformation_time=0.0;
	unsigned long int indexing_time=0.0;

	struct timeval current_time;
    struct timeval transformation_time_start;
    struct timeval indexing_time_start;

    sax_type * sax = malloc(sizeof(sax_type) * ((buffer_data_inmemory*)transferdata)->index->settings->LSH_dimensionality);

    unsigned long start_number=((buffer_data_inmemory*)transferdata)->start_number;
    unsigned long stop_number=((buffer_data_inmemory*)transferdata)->stop_number;
    file_position_type *pos = malloc(sizeof(file_position_type));
    isax_index *index= ((buffer_data_inmemory*)transferdata)->index;
    int LSH_dimensionality=((buffer_data_inmemory*)transferdata)->index->settings->LSH_dimensionality;
    // data_type * lsh = malloc(sizeof(data_type) * LSH_dimensionality);

    unsigned long i=0;

    for (i=start_number;i<stop_number;i++)
    {
        gettimeofday(&transformation_time_start, NULL);

        if(encoding(index, sax, index->lsh_mem_array[i]) == SUCCESS) 
        {
            gettimeofday(&current_time, NULL);
            transformation_time += ((current_time.tv_sec*1000000 + (current_time.tv_usec)) - (transformation_time_start.tv_sec*1000000 + (transformation_time_start.tv_usec)));
            gettimeofday(&indexing_time_start, NULL);

            *pos = (file_position_type)(i*index->settings->data_dimensionality);

            insert_to_index(index, sax, pos, ((buffer_data_inmemory*)transferdata)->lock_firstnode,((buffer_data_inmemory*)transferdata)->workernumber,((buffer_data_inmemory*)transferdata)->total_workernumber);
            
            gettimeofday(&current_time, NULL);
            indexing_time += ((current_time.tv_sec*1000000 + (current_time.tv_usec)) - (indexing_time_start.tv_sec*1000000 + (indexing_time_start.tv_usec)));
        }
        else
        {
            fprintf(stderr, "error: cannot insert record in index, since encoded representation\
                    failed to be created");
        }
    }

    free(pos);
    free(sax);

    pthread_barrier_wait(((buffer_data_inmemory*)transferdata)->lock_barrier1);

    bool have_record=false;
    int j;
    isax_node_record *r = malloc(sizeof(isax_node_record));

    gettimeofday(&indexing_time_start, NULL);

    while(1)
    {

        j=__sync_fetch_and_add(((buffer_data_inmemory*)transferdata)->node_counter,1);

        if(j>=index->fbl->number_of_buffers)
        {
            break;
        }

        parallel_fbl_soft_buffer *current_fbl_node = &((parallel_first_buffer_layer*)(index->fbl))->soft_buffers[j];
        if (!current_fbl_node->initialized) {
            continue;
        }

        int i;
        have_record=false;
        for (int k = 0; k < ((buffer_data_inmemory*)transferdata)->total_workernumber; k++)
        {
        if (current_fbl_node->buffer_size[k] > 0)
            have_record=true;
            
        for (i=0; i<current_fbl_node->buffer_size[k]; i++) 
        {
                r->sax = (sax_type *) &(((current_fbl_node->sax_records[k]))[i*index->settings->LSH_dimensionality]);
                r->position = (file_position_type *) &((file_position_type *)(current_fbl_node->pos_records[k]))[i];
                r->insertion_mode = NO_TMP | PARTIAL;
                add_record_to_node(index, current_fbl_node->node, r, 1);
            }
        }
        if (have_record) 
        {
            flush_subtree_leaf_buffers_inmemory(index, current_fbl_node->node);
        }
        
    }

    gettimeofday(&current_time, NULL);
    indexing_time += ((current_time.tv_sec*1000000 + (current_time.tv_usec)) - (indexing_time_start.tv_sec*1000000 + (indexing_time_start.tv_usec)));

    __sync_fetch_and_add(&TOTAL_INDEXING_PART_TIME,(int)indexing_time);
    __sync_fetch_and_add(&TOTAL_TRANSFORMATION_PART_TIME,(int)transformation_time);

    free(r);
}

root_mask_type insert_to_index(isax_index *index, 
                                    sax_type * sax,
                                    file_position_type * pos,pthread_mutex_t *lock_firstnode,int workernumber,int total_workernumber)
{
    int i,t;
    int totalsize = index->settings->max_total_buffer_size;

    root_mask_type first_bit_mask = 0x00;       

    CREATE_MASK(first_bit_mask, index, sax);
    
    insert_to_pRecBuf((parallel_first_buffer_layer*)(index->fbl), sax, pos,first_bit_mask, index,lock_firstnode,workernumber,total_workernumber);
    return first_bit_mask;
}

enum response flush_subtree_leaf_buffers_inmemory (isax_index *index, isax_node *node)
{
    
    if (node->is_leaf && node->filename != NULL) {

        if (node->buffer->partial_buffer_size > 0 
            || node->buffer->tmp_partial_buffer_size > 0) {
            node->has_partial_data_file = 1;
        }

        if (node->buffer->full_buffer_size > 0 
            || node->buffer->tmp_full_buffer_size > 0) {
            node->has_full_data_file = 1;
        }

        if(node->has_full_data_file) {
            int prev_rec_count = node->leaf_size - (node->buffer->full_buffer_size + node->buffer->tmp_full_buffer_size);
            
            int previous_page_size =  ceil((float) (prev_rec_count * index->settings->full_record_size) / (float) PAGE_SIZE);   
            int current_page_size =   ceil((float) (node->leaf_size * index->settings->full_record_size) / (float) PAGE_SIZE);
            __sync_fetch_and_add(&(index->memory_info.disk_data_full),(current_page_size - previous_page_size));
        }
        if(node->has_partial_data_file) {
            int prev_rec_count = node->leaf_size - (node->buffer->partial_buffer_size + node->buffer->tmp_partial_buffer_size);
            
            int previous_page_size =  ceil((float) (prev_rec_count * index->settings->partial_record_size) / (float) PAGE_SIZE);   
            int current_page_size =   ceil((float) (node->leaf_size * index->settings->partial_record_size) / (float) PAGE_SIZE);
            
            __sync_fetch_and_add(&(index->memory_info.disk_data_partial),(current_page_size - previous_page_size));
        }
        if(node->has_full_data_file && node->has_partial_data_file) {
             printf("WARNING: (Mem size counting) this leaf has both partial and full data.\n");
        }

        __sync_fetch_and_add(&(index->memory_info.disk_data_full),(node->buffer->full_buffer_size + node->buffer->tmp_full_buffer_size));

         __sync_fetch_and_add(&(index->memory_info.disk_data_partial),(node->buffer->partial_buffer_size + node->buffer->tmp_partial_buffer_size));

    }
    else if (!node->is_leaf)
    {
        flush_subtree_leaf_buffers_inmemory(index, node->left_child);
        flush_subtree_leaf_buffers_inmemory(index, node->right_child);
    }
    
    return SUCCESS;
}

isax_index * isax_index_init_inmemory(isax_index_settings *settings)
{
    isax_index *index = malloc(sizeof(isax_index));
    if(index == NULL) {
        fprintf(stderr,"error: could not allocate memory for index structure.\n");
        return NULL;
    }
    index->memory_info.mem_tree_structure = 0;
    index->memory_info.mem_data = 0;
    index->memory_info.mem_summaries = 0;
    index->memory_info.disk_data_full = 0;
    index->memory_info.disk_data_partial = 0;
    
    index->settings = settings;
    index->first_node = NULL;
    index->fbl = initialize_fbl(settings->initial_fbl_buffer_size,
                                pow(2, settings->LSH_dimensionality), 
                                settings->max_total_buffer_size+DISK_BUFFER_SIZE*(PROGRESS_CALCULATE_THREAD_NUMBER-1), index);
  
    index->sax_cache = NULL;
    
    index->total_records = 0;
    index->loaded_records = 0;

    index->root_nodes = 0;
    index->allocated_memory = 0;
    index->has_wedges = 0;

    index->answer = malloc(sizeof(data_type) * settings->data_dimensionality);

    index->norm_factor = ((data_type) 1)/(sqrtf(settings->data_dimensionality));

    return index;
}

isax_node * insert_to_pRecBuf(parallel_first_buffer_layer *fbl, sax_type *sax,
                          file_position_type *pos,root_mask_type mask, 
                          isax_index *index, pthread_mutex_t *lock_firstnode, int workernumber,int total_workernumber) 
{
    parallel_fbl_soft_buffer *current_buffer = &fbl->soft_buffers[(int) mask];

    file_position_type *filepointer;
    sax_type *saxpointer;

    int current_buffer_number;
    char * cd_s,*cd_p;

    // Check if this buffer is initialized
    if (!current_buffer->initialized) 
    {
        pthread_mutex_lock(lock_firstnode);
        if (!current_buffer->initialized) 
        {
            
            current_buffer->max_buffer_size = malloc(sizeof(int)*total_workernumber);
            current_buffer->buffer_size = malloc(sizeof(int)*total_workernumber);
            current_buffer->sax_records=malloc(sizeof(sax_type *)*total_workernumber);
            current_buffer->pos_records=malloc(sizeof(file_position_type *)*total_workernumber);
            for (int i = 0; i < total_workernumber; i++)
            {
                current_buffer->max_buffer_size[i]=0;
                current_buffer->buffer_size[i]=0;
                current_buffer->pos_records[i]=NULL;
                current_buffer->sax_records[i]=NULL;
            }
            current_buffer->node = isax_root_node_init(mask,index->settings->initial_leaf_buffer_size);
            current_buffer->node->is_leaf = 1;
            current_buffer->initialized = 1;
            if(index->first_node == NULL)
            {
                index->first_node = current_buffer->node;
                pthread_mutex_unlock(lock_firstnode);
                current_buffer->node->next = NULL;
                current_buffer->node->previous = NULL;
                
            }
            else
            {
                isax_node * prev_first = index->first_node;
                index->first_node = current_buffer->node;
                index->first_node->next = prev_first;
                prev_first->previous = current_buffer->node;
                pthread_mutex_unlock(lock_firstnode);
            }
            __sync_fetch_and_add(&(index->root_nodes),1);
        }
        else
        {
           pthread_mutex_unlock(lock_firstnode); 
        }
    }  
    
    // Check if this buffer is not full!
    if (current_buffer->buffer_size[workernumber] >= current_buffer->max_buffer_size[workernumber]) {
        if(current_buffer->max_buffer_size[workernumber] == 0) {
            current_buffer->max_buffer_size[workernumber] = fbl->initial_buffer_size;
            current_buffer->sax_records[workernumber] = malloc(index->settings->sax_byte_size * 
                                                 current_buffer->max_buffer_size[workernumber]);
            current_buffer->pos_records[workernumber] = malloc(index->settings->position_byte_size* 
                                                 current_buffer->max_buffer_size[workernumber]);
        }
        else {
            current_buffer->max_buffer_size[workernumber] *= BUFFER_REALLOCATION_RATE;
            
            current_buffer->sax_records[workernumber] = realloc(current_buffer->sax_records[workernumber],
                                           index->settings->sax_byte_size * 
                                           current_buffer->max_buffer_size[workernumber]);
            current_buffer->pos_records[workernumber] = realloc(current_buffer->pos_records[workernumber],
                                           index->settings->position_byte_size *
                                           current_buffer->max_buffer_size[workernumber]);
            
        }
    }

    if (current_buffer->sax_records[workernumber] == NULL || current_buffer->pos_records[workernumber] == NULL) {
        fprintf(stderr, "error: Could not allocate memory in FBL.");
        return OUT_OF_MEMORY_FAILURE;
    }
    
    current_buffer_number=current_buffer->buffer_size[workernumber];
    filepointer=(file_position_type *)current_buffer->pos_records[workernumber];
    saxpointer=(sax_type *)current_buffer->sax_records[workernumber];

    memcpy((void *) (&saxpointer[current_buffer_number*index->settings->LSH_dimensionality]), (void *) sax, index->settings->sax_byte_size);
    memcpy((void *) (&filepointer[current_buffer_number]), (void *) pos, index->settings->position_byte_size);
    
   
    #ifdef DEBUG
    printf("*** Added to node ***\n\n");
    #ifdef TOY
    sax_print(sax, index->settings->LSH_dimensionality, 
              index->settings->sax_bit_cardinality);
    #endif
    #endif

    (current_buffer->buffer_size[workernumber])++;

    return current_buffer->node;
}


