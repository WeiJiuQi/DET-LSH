#include "../../../config.h"
#include "../../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils/sax/sax.h"
#include "utils/isax/isax_first_buffer_layer.h"

struct first_buffer_layer * initialize_fbl(int initial_buffer_size, int number_of_buffers,
                                           int max_total_buffers_size, isax_index *index) 
{
    struct first_buffer_layer *fbl = malloc(sizeof(first_buffer_layer));
    
    fbl->max_total_size = max_total_buffers_size;
    fbl->initial_buffer_size = initial_buffer_size;
    fbl->number_of_buffers = number_of_buffers;
    
    // Allocate a big chunk of memory to store sax data and positions
    long long hard_buffer_size = (long long)(index->settings->sax_byte_size + index->settings->position_byte_size) * (long long) max_total_buffers_size;
    fbl->hard_buffer = malloc(hard_buffer_size);
    
    if(fbl->hard_buffer == NULL) {
	fprintf(stderr, "Could not initialize hard buffer of size: %lld\n", hard_buffer_size);
	exit(-1);	
    } 
           
    // Allocate a set of soft buffers to hold pointers to the hard buffer
    fbl->soft_buffers = malloc(sizeof(parallel_fbl_soft_buffer) * number_of_buffers);
    fbl->current_record_index = 0;
    fbl->current_record = fbl->hard_buffer;
    int i;
    for (i=0; i<number_of_buffers; i++) {
        fbl->soft_buffers[i].initialized = 0;
        fbl->soft_buffers[i].max_buffer_size = 0;
        fbl->soft_buffers[i].buffer_size = 0;
    }
    return fbl;
}
struct parallel_first_buffer_layer * initialize_pRecBuf(int initial_buffer_size, int number_of_buffers,
                                           int max_total_buffers_size, isax_index *index) 
{
    struct parallel_first_buffer_layer *fbl = malloc(sizeof(parallel_first_buffer_layer));
    
    fbl->max_total_size = max_total_buffers_size;
    fbl->initial_buffer_size = initial_buffer_size;
    fbl->number_of_buffers = number_of_buffers;
    
    // Allocate a big chunk of memory to store sax data and positions
    long long hard_buffer_size = (long long)(index->settings->sax_byte_size + index->settings->position_byte_size) * (long long) max_total_buffers_size;
    //fbl->hard_buffer = malloc(hard_buffer_size);
           
    // Allocate a set of soft buffers to hold pointers to the hard buffer
    fbl->soft_buffers = malloc(sizeof(parallel_fbl_soft_buffer) * number_of_buffers);
    fbl->current_record_index = 0;
    int i;
    for (i=0; i<number_of_buffers; i++) {
        fbl->soft_buffers[i].initialized = 0;
        fbl->soft_buffers[i].finished = 0;
    }
    return fbl;
}

isax_node * insert_to_fbl(first_buffer_layer *fbl, sax_type *sax,
                          file_position_type *pos,root_mask_type mask, 
                          isax_index *index) 
{
    
    fbl_soft_buffer *current_buffer = &fbl->soft_buffers[(int) mask];
    
    // Check if this buffer is initialized
    if (!current_buffer->initialized) {
    #ifdef DEBUG
        printf("*** Creating new FBL node. ***\n\n");
    #endif
        current_buffer->initialized = 1;
        current_buffer->max_buffer_size = 0;
        current_buffer->buffer_size = 0;
        
        current_buffer->node = isax_root_node_init(mask, 
                                                   index->settings->initial_leaf_buffer_size);
        index->root_nodes++;
        current_buffer->node->is_leaf = 1;
        
        if(index->first_node == NULL)
        {
            index->first_node = current_buffer->node;
            current_buffer->node->next = NULL;
            current_buffer->node->previous = NULL;
        }
        else
        {
            isax_node * prev_first = index->first_node;
            index->first_node = current_buffer->node;
            index->first_node->next = prev_first;
            prev_first->previous = current_buffer->node;
        }
    }  
    // Check if this buffer is not full!
    if (current_buffer->buffer_size >= current_buffer->max_buffer_size) {
        
        if(current_buffer->max_buffer_size == 0) {
            current_buffer->max_buffer_size = fbl->initial_buffer_size;
            
            current_buffer->max_buffer_size = fbl->initial_buffer_size;
            
            current_buffer->sax_records = malloc(sizeof(sax_type *) * 
                                                 current_buffer->max_buffer_size);
            current_buffer->pos_records = malloc(sizeof(file_position_type *) * 
                                                 current_buffer->max_buffer_size);
        }
        else {
            current_buffer->max_buffer_size *= BUFFER_REALLOCATION_RATE;
            
            current_buffer->sax_records = realloc(current_buffer->sax_records,
                                           sizeof(sax_type *) * 
                                           current_buffer->max_buffer_size);
            current_buffer->pos_records = realloc(current_buffer->pos_records,
                                           sizeof(file_position_type *) * 
                                           current_buffer->max_buffer_size);
            
        }
    }
    if (current_buffer->sax_records == NULL || current_buffer->pos_records == NULL) {
        fprintf(stderr, "error: Could not allocate memory in FBL.");
        return OUT_OF_MEMORY_FAILURE;
    }

    // Copy data to hard buffer and make current buffer point to the hard one
    current_buffer->sax_records[current_buffer->buffer_size] = (sax_type*) fbl->current_record;    
    memcpy((void *) fbl->current_record, (void *) sax, index->settings->sax_byte_size);
    fbl->current_record += index->settings->sax_byte_size;
    
    current_buffer->pos_records[current_buffer->buffer_size] = (file_position_type*) fbl->current_record;    
    memcpy((void *) fbl->current_record, (void *) pos, index->settings->position_byte_size);
    fbl->current_record += index->settings->position_byte_size;
    
    #ifdef DEBUG
    printf("*** Added to node ***\n\n");
    #ifdef TOY
    sax_print(sax, index->settings->LSH_dimensionality, 
              index->settings->sax_bit_cardinality);
    #endif
    #endif
    current_buffer->buffer_size++;
    fbl->current_record_index++;

    return current_buffer->node;
}



enum response flush_fbl(first_buffer_layer *fbl, isax_index *index) 
{
    #ifdef DEBUG
    printf("*** FLUSHING ***\n\n");
    #else
    #if VERBOSE_LEVEL == 2
    printf("\n");
    fflush(stdout);
    int i=1;
    fprintf(stdout, "\x1b[31mFlushing: \x1b[36m00.00%%\x1b[0m");
    #endif
    #if VERBOSE_LEVEL == 1
    printf("\n");
    fflush(stdout);
    int i=1;
    fprintf(stdout, "Flushing...\n");
    #endif    
    #endif
    
    int c = 1;
    int j;
    isax_node_record *r = malloc(sizeof(isax_node_record));
    for (j=0; j<fbl->number_of_buffers; j++) 
    {
        
        fbl_soft_buffer *current_fbl_node = &index->fbl->soft_buffers[j];
        if (!current_fbl_node->initialized) {
            continue;
        }
        
    #ifndef DEBUG
    #if VERBOSE_LEVEL == 2
        fprintf(stdout,"\r\x1b[31mFlushing: \x1b[36m%2.2lf%%\x1b[0m", ((float)c/(float)index->root_nodes)*100);
        c++;
        fflush(stdout);
    #endif
    #endif
        int i;
        if (current_fbl_node->buffer_size > 0) 
        {
            // For all records in this buffer 
            //COUNT_CAL_TIME_START
            for (i=0; i<current_fbl_node->buffer_size; i++) {
                r->sax = (sax_type *) current_fbl_node->sax_records[i];
                r->position = (file_position_type *) current_fbl_node->pos_records[i];
                r->insertion_mode = NO_TMP | PARTIAL;
                // Add record to index
                
                add_record_to_node(index, current_fbl_node->node, r, 1);
                
            }
            
            // flush index node
            //COUNT_CAL_TIME_START
            flush_subtree_leaf_buffers(index, current_fbl_node->node);
            //COUNT_CAL_TIME_END
            // clear FBL records moved in LBL buffers
            free(current_fbl_node->sax_records);
            free(current_fbl_node->pos_records);
            // clear records read from files (free only prev sax buffers)
            
            isax_index_clear_node_buffers(index, current_fbl_node->node, 
                                          INCLUDE_CHILDREN,
                                          TMP_AND_TS_CLEAN);
            
            index->allocated_memory = 0;
            // Set to 0 in order to re-allocate original space for buffers.
            current_fbl_node->buffer_size = 0;
            current_fbl_node->max_buffer_size = 0;
        }
        
    }

    free(r);
    fbl->current_record_index = 0;
    fbl->current_record = fbl->hard_buffer;
    #ifndef DEBUG
    #if VERBOSE_LEVEL == 2
    printf("\n");
    #endif
    #endif
    
    return SUCCESS;
} 


void destroy_fbl(first_buffer_layer *fbl) {

    // free(fbl->hard_buffer);
    free(fbl->soft_buffers);
    free(fbl);
}

void destroy_pRecBuf(parallel_first_buffer_layer *fbl,int prewokernumber) {

    for (int j=0; j<fbl->number_of_buffers; j++) 
    {
        parallel_fbl_soft_buffer *current_fbl_node = &fbl->soft_buffers[j];
        if (!current_fbl_node->initialized) {
            continue;
        }
        for (int k = 0; k < prewokernumber; k++)
        {   
            if(current_fbl_node->sax_records[k]!=NULL)
            {
                free((current_fbl_node->sax_records[k]));
                free((current_fbl_node->pos_records[k]));
                current_fbl_node->sax_records[k]=NULL;
                current_fbl_node->pos_records[k]=NULL;
            }  
        }
        free(current_fbl_node->buffer_size);
        free(current_fbl_node->max_buffer_size);
        free(current_fbl_node->sax_records);
        free(current_fbl_node->pos_records);
    }
    free(fbl->soft_buffers);
    free(fbl);
}


void destroy_pRecBufdet(parallel_first_buffer_layer *fbl,int prewokernumber) {

    for (int j=0; j<fbl->number_of_buffers; j++) 
    {
        parallel_fbl_soft_buffer *current_fbl_node = &fbl->soft_buffers[j];
        if (!current_fbl_node->initialized) {
            continue;
        }
        for (int k = 0; k < prewokernumber; k++)
        {   
            if(current_fbl_node->sax_records[k]!=NULL)
            {
                free((current_fbl_node->sax_records[k]));
                current_fbl_node->sax_records[k]=NULL;
            }  
        }
        free(current_fbl_node->sax_records);
    }
}