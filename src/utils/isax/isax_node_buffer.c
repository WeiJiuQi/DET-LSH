#include "../../../config.h"
#include "../../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "utils/isax/isax_node_buffer.h"
#include "utils/isax/isax_node.h"
#include "utils/isax/isax_node_record.h"

void destroy_node_buffer(isax_node_buffer *node_buffer) {
    if (node_buffer->full_position_buffer != NULL) {
        free(node_buffer->full_position_buffer);
        node_buffer->full_position_buffer = NULL;
    }
    if (node_buffer->full_sax_buffer != NULL) {
        free(node_buffer->full_sax_buffer);
        node_buffer->full_sax_buffer = NULL;
    }
    if (node_buffer->full_ts_buffer != NULL) {
        free(node_buffer->full_ts_buffer);
        node_buffer->full_ts_buffer = NULL;
    }
    if (node_buffer->partial_position_buffer != NULL) {
        // !!! DON'T FREE THAT IT REMOVES THE DATA!!!!
        free(node_buffer->partial_position_buffer);
        node_buffer->partial_position_buffer = NULL;
    }
    if (node_buffer->partial_sax_buffer != NULL) {
        free(node_buffer->partial_sax_buffer);
        node_buffer->partial_sax_buffer = NULL;
    }
    if (node_buffer->tmp_full_position_buffer != NULL) {
        free(node_buffer->tmp_full_position_buffer);
        node_buffer->tmp_full_position_buffer = NULL;
    }
    if (node_buffer->tmp_full_sax_buffer != NULL) {
        free(node_buffer->tmp_full_sax_buffer);
        node_buffer->tmp_full_sax_buffer = NULL;
    }
    if (node_buffer->tmp_full_ts_buffer != NULL) {
        free(node_buffer->tmp_full_ts_buffer);
        node_buffer->tmp_full_ts_buffer = NULL;
    }
    if (node_buffer->tmp_partial_position_buffer != NULL) {
        free(node_buffer->tmp_partial_position_buffer);
        node_buffer->tmp_partial_position_buffer = NULL;
    }
    if (node_buffer->tmp_partial_sax_buffer != NULL) {
        free(node_buffer->tmp_partial_sax_buffer);
        node_buffer->tmp_partial_sax_buffer = NULL;
    }
    free(node_buffer);
}


isax_node_buffer * init_node_buffer(int initial_buffer_size) {
    isax_node_buffer * node_buffer = malloc(sizeof(isax_node_buffer));
    node_buffer->initial_buffer_size = initial_buffer_size;
    
    node_buffer->max_full_buffer_size = 0;
    node_buffer->max_partial_buffer_size = 0;
    node_buffer->max_tmp_full_buffer_size = 0;
    node_buffer->max_tmp_partial_buffer_size = 0;
    node_buffer->full_buffer_size = 0;
    node_buffer->partial_buffer_size = 0;
    node_buffer->tmp_full_buffer_size = 0;
    node_buffer->tmp_partial_buffer_size = 0;
    
    (node_buffer->full_position_buffer) = NULL;
    (node_buffer->full_sax_buffer) = NULL;
    (node_buffer->full_ts_buffer) = NULL;
    (node_buffer->partial_position_buffer) = NULL;
    (node_buffer->partial_sax_buffer) = NULL;
    (node_buffer->tmp_full_position_buffer) = NULL;
    (node_buffer->tmp_full_sax_buffer) = NULL;
    (node_buffer->tmp_full_ts_buffer) = NULL;
    (node_buffer->tmp_partial_position_buffer) = NULL;
    (node_buffer->tmp_partial_sax_buffer = NULL);   
    
    return node_buffer;
}

enum response add_to_node_buffer(isax_node_buffer *node_buffer, 
                                 isax_node_record *record, 
                                 int sax_segments, int ts_segments)
{
    if (record->insertion_mode & TMP) 
    {
        if (record->insertion_mode & FULL) {
            if (node_buffer->max_tmp_full_buffer_size == 0) {
                node_buffer->max_tmp_full_buffer_size = node_buffer->initial_buffer_size;
                node_buffer->tmp_full_position_buffer = malloc(sizeof(file_position_type*) * 
                                                               node_buffer->max_tmp_full_buffer_size);
                node_buffer->tmp_full_sax_buffer = malloc(sizeof(sax_type*) * 
                                                          node_buffer->max_tmp_full_buffer_size);
                node_buffer->tmp_full_ts_buffer = malloc(sizeof(data_type*) * 
                                                         node_buffer->max_tmp_full_buffer_size);
            }
            else if (node_buffer->max_tmp_full_buffer_size <= node_buffer->tmp_full_buffer_size) {
                node_buffer->max_tmp_full_buffer_size *= BUFFER_REALLOCATION_RATE;
                node_buffer->tmp_full_position_buffer = realloc(node_buffer->tmp_full_position_buffer,
                                                                sizeof(file_position_type*) * 
                                                                node_buffer->max_tmp_full_buffer_size);
                node_buffer->tmp_full_sax_buffer = realloc(node_buffer->tmp_full_sax_buffer,
                                                           sizeof(sax_type*) * 
                                                           node_buffer->max_tmp_full_buffer_size);
                node_buffer->tmp_full_ts_buffer = realloc(node_buffer->tmp_full_ts_buffer,
                                                          sizeof(data_type*) * 
                                                          node_buffer->max_tmp_full_buffer_size);
            }
            node_buffer->tmp_full_position_buffer[node_buffer->tmp_full_buffer_size] = record->position;
            node_buffer->tmp_full_sax_buffer[node_buffer->tmp_full_buffer_size] = record->sax;
            node_buffer->tmp_full_ts_buffer[node_buffer->tmp_full_buffer_size] = record->ts;
            node_buffer->tmp_full_buffer_size++;
        }
        if (record->insertion_mode & PARTIAL) {
            if (node_buffer->max_tmp_partial_buffer_size == 0) {
                node_buffer->max_tmp_partial_buffer_size = node_buffer->initial_buffer_size;
                node_buffer->tmp_partial_position_buffer = malloc(sizeof(file_position_type*) * 
                                                               node_buffer->max_tmp_partial_buffer_size);
                node_buffer->tmp_partial_sax_buffer = malloc(sizeof(sax_type*) * 
                                                          node_buffer->max_tmp_partial_buffer_size);
            }
            else if (node_buffer->max_tmp_partial_buffer_size <= node_buffer->tmp_partial_buffer_size) {
                node_buffer->max_tmp_partial_buffer_size *= BUFFER_REALLOCATION_RATE;
                node_buffer->tmp_partial_position_buffer = realloc(node_buffer->tmp_full_position_buffer,
                                                                sizeof(file_position_type*) * 
                                                                node_buffer->max_tmp_partial_buffer_size);
                node_buffer->tmp_partial_sax_buffer = realloc(node_buffer->tmp_full_sax_buffer,
                                                           sizeof(sax_type*) * 
                                                           node_buffer->max_tmp_partial_buffer_size);
            }
            node_buffer->tmp_partial_position_buffer[node_buffer->tmp_partial_buffer_size] = record->position;
            node_buffer->tmp_partial_sax_buffer[node_buffer->tmp_partial_buffer_size] = record->sax;
            node_buffer->tmp_partial_buffer_size++;
        }
    }
    else if (record->insertion_mode & NO_TMP)
    {
        if (record->insertion_mode & FULL) {
            if (node_buffer->max_full_buffer_size == 0) {
                node_buffer->max_full_buffer_size = node_buffer->initial_buffer_size;
                node_buffer->full_position_buffer = malloc(sizeof(file_position_type*) * 
                                                           node_buffer->max_full_buffer_size);
                node_buffer->full_sax_buffer = malloc(sizeof(sax_type*) * 
                                                      node_buffer->max_full_buffer_size);
                node_buffer->full_ts_buffer = malloc(sizeof(data_type*) * 
                                                     node_buffer->max_full_buffer_size);
            }
            else if (node_buffer->max_full_buffer_size <= node_buffer->full_buffer_size) {
                node_buffer->max_full_buffer_size *= BUFFER_REALLOCATION_RATE;
                node_buffer->full_position_buffer = realloc(node_buffer->full_position_buffer,
                                                            sizeof(file_position_type*) * 
                                                            node_buffer->max_full_buffer_size);
                node_buffer->full_sax_buffer = realloc(node_buffer->full_sax_buffer,
                                                       sizeof(sax_type*) * 
                                                       node_buffer->max_full_buffer_size);
                node_buffer->full_ts_buffer = realloc(node_buffer->full_ts_buffer,
                                                      sizeof(data_type*) * 
                                                      node_buffer->max_full_buffer_size);
            }
            node_buffer->full_position_buffer[node_buffer->full_buffer_size] = record->position;
            node_buffer->full_sax_buffer[node_buffer->full_buffer_size] = record->sax;
            node_buffer->full_ts_buffer[node_buffer->full_buffer_size] = record->ts;
            node_buffer->full_buffer_size++;
        }
        if (record->insertion_mode & PARTIAL) {
            if (node_buffer->max_partial_buffer_size == 0) {
                node_buffer->max_partial_buffer_size = node_buffer->initial_buffer_size;
                node_buffer->partial_position_buffer = malloc(sizeof(file_position_type*) * 
                                                              node_buffer->max_partial_buffer_size);
                node_buffer->partial_sax_buffer = malloc(sizeof(sax_type*) * 
                                                         node_buffer->max_partial_buffer_size);
            }
            else if (node_buffer->max_partial_buffer_size <= node_buffer->partial_buffer_size) {
                node_buffer->max_partial_buffer_size *= BUFFER_REALLOCATION_RATE;
                node_buffer->partial_position_buffer = realloc(node_buffer->partial_position_buffer,
                                                               sizeof(file_position_type*) * 
                                                               node_buffer->max_partial_buffer_size);
                node_buffer->partial_sax_buffer = realloc(node_buffer->partial_sax_buffer,
                                                          sizeof(sax_type*) * 
                                                          node_buffer->max_partial_buffer_size);
            }
            node_buffer->partial_position_buffer[node_buffer->partial_buffer_size] = record->position;
            node_buffer->partial_sax_buffer[node_buffer->partial_buffer_size] = record->sax;
            node_buffer->partial_buffer_size++;
        } 
    }
    
    return SUCCESS;
}

enum response flush_node_buffer(isax_node_buffer *node_buffer, 
                                int sax_segments, int ts_segments, 
                                const char * filename) 
{
    // WRITE TWO DIFFERENT FILES!
    // 1. .FULL (full records)
    // 2. .PART (partial records)
    int i;
    if (node_buffer->full_buffer_size > 0 
        || node_buffer->tmp_full_buffer_size > 0)
    {
        FILE * full_file;
        char * full_filename = malloc(strlen(filename)+6);
        sprintf(full_filename, "%s.full", filename);
        COUNT_OUTPUT_TIME_START
        full_file = fopen(full_filename, "a+");
        // Flushing full records
        for (i = 0; i < node_buffer->full_buffer_size; i++) {;
            fwrite(node_buffer->full_position_buffer[i], 
                   sizeof(file_position_type), 1, full_file);
            fwrite(node_buffer->full_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, full_file);
            fwrite(node_buffer->full_ts_buffer[i], 
                   sizeof(data_type), ts_segments, full_file);
        }
        for (i = 0; i < node_buffer->tmp_full_buffer_size; i++) {
            fwrite(node_buffer->tmp_full_position_buffer[i], 
                   sizeof(file_position_type), 1, full_file);
            fwrite(node_buffer->tmp_full_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, full_file);
            fwrite(node_buffer->tmp_full_ts_buffer[i], 
                   sizeof(data_type), ts_segments, full_file);
        } 
        fclose(full_file);
        COUNT_OUTPUT_TIME_END
        free(full_filename);
    }
     
    if (node_buffer->partial_buffer_size > 0 
        || node_buffer->tmp_partial_buffer_size > 0) {
        FILE * partial_file;
        char * partial_filename = malloc(strlen(filename)+6);
        //sax_type *midium_ptr=malloc(sizeof(sax_type)*(sax_segments+8));
        sprintf(partial_filename, "%s.part", filename);
        COUNT_OUTPUT_TIME_START
        partial_file = fopen(partial_filename, "a+");
        // Flushing partial records
        //printf("the large is %d\n",node_buffer->partial_buffer_size);
        for (i = 0; i < node_buffer->partial_buffer_size; i++) {
            //memcpy(midium_ptr, (sax_type*)(node_buffer->partial_position_buffer[i]),8);
            //memcpy(&(midium_ptr[8]), (sax_type*)(node_buffer->partial_sax_buffer[i]),sax_segments);
            //fwrite(midium_ptr, sizeof(sax_type), sax_segments+8, partial_file);
            //memset(midium_ptr,0,sizeof(sax_type)+8);

            fwrite(node_buffer->partial_position_buffer[i], 
                   sizeof(file_position_type), 1, partial_file);
            fwrite(node_buffer->partial_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, partial_file);

        }
            //printf("hello word\n");
        for (i = 0; i < node_buffer->tmp_partial_buffer_size; i++) {
            //memcpy(midium_ptr, (sax_type*)(node_buffer->tmp_partial_position_buffer[i]),8);
            //memcpy(&(midium_ptr[8]), (sax_type*)(node_buffer->tmp_partial_sax_buffer[i]),sax_segments);
            //fwrite(midium_ptr, sizeof(sax_type), sax_segments+8, partial_file);
            //memset(midium_ptr,0,sizeof(sax_type)+8);

            fwrite(node_buffer->tmp_partial_position_buffer[i], 
                   sizeof(file_position_type), 1, partial_file);
            fwrite(node_buffer->tmp_partial_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, partial_file);
        }

        //free(midium_ptr);
        fclose(partial_file);
        COUNT_OUTPUT_TIME_END
        
        free(partial_filename);
    }
    return SUCCESS;
}
enum response flush_node_buffer_m(isax_node_buffer *node_buffer, 
                                int sax_segments, int ts_segments, 
                                const char * filename, pthread_mutex_t *lock_write) 
{
    // WRITE TWO DIFFERENT FILES!
    // 1. .FULL (full records)
    // 2. .PART (partial records)
    int i;
        int plotconter=0;

    if (node_buffer->full_buffer_size > 0 
        || node_buffer->tmp_full_buffer_size > 0)
    {
        FILE * full_file;
        char * full_filename = malloc(strlen(filename)+6);
        sprintf(full_filename, "%s.full", filename);
        
        //pthread_mutex_lock(lock_write);
        //COUNT_OUTPUT_TIME_START
        full_file = fopen(full_filename, "a+");
        // Flushing full records
        
        for (i = 0; i < node_buffer->full_buffer_size; i++) {;
            fwrite(node_buffer->full_position_buffer[i], 
                   sizeof(file_position_type), 1, full_file);
            fwrite(node_buffer->full_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, full_file);
            fwrite(node_buffer->full_ts_buffer[i], 
                   sizeof(data_type), ts_segments, full_file);
        }
        //plotconter=plotconter+node_buffer->full_buffer_size*(sizeof(file_position_type)+sizeof(sax_type)*sax_segments+sizeof(data_type)*ts_segments);
        //printf("wfjleklefjwfelwej\n");
       for (i = 0; i < node_buffer->tmp_full_buffer_size; i++) {
            fwrite(node_buffer->tmp_full_position_buffer[i], 
                   sizeof(file_position_type), 1, full_file);
            fwrite(node_buffer->tmp_full_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, full_file);
            fwrite(node_buffer->tmp_full_ts_buffer[i], 
                   sizeof(data_type), ts_segments, full_file);

        } 
        //plotconter=plotconter+node_buffer->tmp_full_buffer_size*(sizeof(file_position_type)+sizeof(sax_type)*sax_segments+sizeof(data_type)*ts_segments);

        fclose(full_file);
        //COUNT_OUTPUT_TIME_END
        //pthread_mutex_unlock(lock_write);
        
        free(full_filename);
    }
     
    if (node_buffer->partial_buffer_size > 0 
        || node_buffer->tmp_partial_buffer_size > 0) {
        FILE * partial_file;
        char * partial_filename = malloc(strlen(filename)+6);
        //sax_type *midium_ptr=malloc(sizeof(sax_type)*(sax_segments+8));
        sprintf(partial_filename, "%s.part", filename);

        //pthread_mutex_lock(lock_write);
        //COUNT_OUTPUT_TIME_START
        partial_file = fopen(partial_filename, "a+");
        // Flushing partial records
        //printf("the large is %d\n",node_buffer->partial_buffer_size);
        for (i = 0; i < node_buffer->partial_buffer_size; i++) {
            //memcpy(midium_ptr, (sax_type*)(node_buffer->partial_position_buffer[i]),8);
            //memcpy(&(midium_ptr[8]), (sax_type*)(node_buffer->partial_sax_buffer[i]),sax_segments);
            //fwrite(midium_ptr, sizeof(sax_type), sax_segments+8, partial_file);
            //memset(midium_ptr,0,sizeof(sax_type)+8);

            fwrite(node_buffer->partial_position_buffer[i], 
                   sizeof(file_position_type), 1, partial_file);
            fwrite(node_buffer->partial_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, partial_file);

        }
                //plotconter=plotconter+node_buffer->partial_buffer_size*(sizeof(file_position_type)+sizeof(sax_type)*sax_segments);

            //printf("hello word\n");
        for (i = 0; i < node_buffer->tmp_partial_buffer_size; i++) {
            //memcpy(midium_ptr, (sax_type*)(node_buffer->tmp_partial_position_buffer[i]),8);
            //memcpy(&(midium_ptr[8]), (sax_type*)(node_buffer->tmp_partial_sax_buffer[i]),sax_segments);
            //fwrite(midium_ptr, sizeof(sax_type), sax_segments+8, partial_file);
            //memset(midium_ptr,0,sizeof(sax_type)+8);

            fwrite(node_buffer->tmp_partial_position_buffer[i], 
                   sizeof(file_position_type), 1, partial_file);
            fwrite(node_buffer->tmp_partial_sax_buffer[i], 
                   sizeof(sax_type), sax_segments, partial_file);
        } 
        //free(midium_ptr);
        fclose(partial_file);


        free(partial_filename);
    }
    return SUCCESS;
}
enum response clear_node_buffer(isax_node_buffer *node_buffer,
                                enum buffer_cleaning_mode clean_mode)
{
    
    // ONLY SELECTIVELY CLEAR STUFF THAT COME FROM THE INSERT 
    // TO INDEX FUNCTION BECAUSE THEY MAY COME FROM REUSABLE 
    // MEMORY SEGMENTS SUCH AS THE FBL BUFFERS WHICH ARE USED
    // AGAIN FOR THE NEXT EPOC OF DATA LOADING WITHOUT BEING 
    // FREED.
    int i;
    if (clean_mode == FULL_CLEAN || clean_mode == TMP_AND_TS_CLEAN)
    for (i = 0; i < node_buffer->full_buffer_size; i++) {
        if (clean_mode == FULL_CLEAN || clean_mode == TMP_AND_TS_CLEAN)
        {
            // IMPORTANT: THIS MEANS THAT NOBODY SHOULD INSERT A FULL TS FROM SHARED MEMORY
            // THE TS_BUFFER WILL *ALWAYS* BE CLEARED! AND AN INVALID FREE WILL BE RAISED!
            free(node_buffer->full_ts_buffer[i]);
        }
        
        if (clean_mode == FULL_CLEAN) {
            free(node_buffer->full_sax_buffer[i]);
            free(node_buffer->full_position_buffer[i]);
        }
    }
       
    if (clean_mode == FULL_CLEAN)
    for (i = 0; i < node_buffer->partial_buffer_size; i++) {
        if (clean_mode == FULL_CLEAN) {
            free(node_buffer->partial_sax_buffer[i]);
            free(node_buffer->partial_position_buffer[i]);
        }
    }
    
    // FULL CLEAN TEMPORARY STUFF
    for (i = 0; i < node_buffer->tmp_full_buffer_size; i++) {
        free(node_buffer->tmp_full_ts_buffer[i]);
        free(node_buffer->tmp_full_sax_buffer[i]);
        free(node_buffer->tmp_full_position_buffer[i]);
    } 
    for (i = 0; i < node_buffer->tmp_partial_buffer_size; i++) {
        free(node_buffer->tmp_partial_sax_buffer[i]);
        free(node_buffer->tmp_partial_position_buffer[i]);
    }
    
    // Set to 0 so that the LBLs will be refilled.
    
    node_buffer->tmp_full_buffer_size = 0;
    node_buffer->full_buffer_size = 0;
    node_buffer->tmp_partial_buffer_size = 0;
    node_buffer->partial_buffer_size = 0;
    
    
    return SUCCESS;
}
