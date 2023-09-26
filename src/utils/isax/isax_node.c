#include "../../../config.h"
#include "../../../globals.h"
#include <stdio.h>
#include <stdlib.h>

#include "utils/isax/isax_node.h"

/**
 This function initializes an isax root node.
 */
isax_node * isax_root_node_init(root_mask_type mask, int initial_buffer_size) 
{
    isax_node *node = isax_leaf_node_init(initial_buffer_size);
    node->mask = mask;
    return node;
}

/**
 This function initalizes an isax leaf node.
 */
isax_node * isax_leaf_node_init(int initial_buffer_size) 
{
    COUNT_NEW_NODE()
    isax_node *node = malloc(sizeof(isax_node));
    if(node == NULL) {
        fprintf(stderr,"error: could not allocate memory for new node.\n");
        return NULL;
    }
    node->has_partial_data_file = 0;
    node->has_full_data_file = 0;
    node->right_child = NULL;
    node->left_child = NULL;
    node->parent = NULL;
    node->next = NULL;
    node->leaf_size = 0;
    node->filename = NULL;
    node->isax_values = NULL;
    node->isax_cardinalities = NULL;
    node->previous = NULL;
    node->split_data = NULL;
    node->buffer = init_node_buffer(initial_buffer_size);
    node->mask = 0;
    node->wedges = NULL;
    return node;
}



