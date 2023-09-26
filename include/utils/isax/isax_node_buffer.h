#ifndef aisax_isax_node_buffer_h
#define aisax_isax_node_buffer_h
#include "../../../config.h"
#include "../../../globals.h"
#include "isax_node_record.h"

typedef struct isax_node_buffer {
    int initial_buffer_size;
    
    // Full data:
    sax_type **full_sax_buffer;
    file_position_type **full_position_buffer;
    data_type **full_ts_buffer;
    
    // Partial data:
    sax_type **partial_sax_buffer;
    file_position_type **partial_position_buffer;
    
    // TMP Buffers (stuff coming from splits and loutils)
    // Full data:
    sax_type **tmp_full_sax_buffer;
    file_position_type **tmp_full_position_buffer;
    data_type **tmp_full_ts_buffer;
    
    // Partial data:
    sax_type **tmp_partial_sax_buffer;
    file_position_type **tmp_partial_position_buffer;
    
    
    int full_buffer_size;
    int max_full_buffer_size;
    
    int partial_buffer_size;
    int max_partial_buffer_size;
    
    int tmp_full_buffer_size;
    int max_tmp_full_buffer_size;
    
    int tmp_partial_buffer_size;
    int max_tmp_partial_buffer_size;

} isax_node_buffer;

void destroy_node_buffer(isax_node_buffer *buffer);

isax_node_buffer * init_node_buffer(int initial_buffer_size);

enum response add_to_node_buffer(isax_node_buffer *node_buffer, 
                                 isax_node_record *record, 
                                 int sax_segments, int ts_segments);
enum response flush_node_buffer(isax_node_buffer *node_buffer, 
                                int sax_segments, int ts_segments, 
                                const char * filename);
enum response flush_node_buffer_m(isax_node_buffer *node_buffer, 
                                int sax_segments, int ts_segments, 
                                const char * filename, pthread_mutex_t *lock_write);
enum response clear_node_buffer(isax_node_buffer *node_buffer, 
                                enum buffer_cleaning_mode clean_mode);
#endif
