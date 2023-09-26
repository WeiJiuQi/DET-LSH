#ifndef isaxlib_isax_node_h
#define isaxlib_isax_node_h

struct isax_node;

#include "../../../config.h"
#include "../../../globals.h"
#include "isax_node_record.h"
#include "isax_node_buffer.h"

typedef struct isax_node_split_data {
    int splitpoint;
    sax_type * split_mask;
} isax_node_split_data;

typedef struct isax_node {
    // General
    int leaf_size;
    char has_partial_data_file;
    char has_full_data_file;
    
    sax_type * isax_values;
    sax_type * isax_cardinalities;
    
    struct isax_node *next;
    struct isax_node *previous;
    root_mask_type mask;
    struct isax_node *parent;
    
    // If is leaf
    unsigned char is_leaf; 
    char * filename;
    isax_node_buffer *buffer;
    
    // If is intermediate
    struct isax_node_split_data *split_data;
    struct isax_node *left_child;
    struct isax_node *right_child;
    
    // Wedges
    data_type *wedges;

} isax_node;


isax_node * isax_root_node_init(root_mask_type mask, int initial_buffer_size);
isax_node * isax_leaf_node_init(int initial_buffer_size);

#endif
