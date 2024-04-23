#ifdef VALUES
#include <values.h>
#endif

#include "../../../config.h"
#include "../../../globals.h"
	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdbool.h>

#include "utils/sax/sax.h"
#include "utils/sax/ts.h"
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_node.h"
#include "utils/isax/isax_node_buffer.h"
#include "utils/isax/isax_node_split.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/isax/pqueue.h"
/**
 This function initializes the settings of an isax index
 */
isax_index_settings * isax_index_settings_init(const char * root_directory, int data_dimensionality, 
                                                int LSH_dimensionality, int sax_bit_cardinality, 
                                                int max_leaf_size, int min_leaf_size,
                                                int initial_leaf_buffer_size,
                                                int max_total_buffer_size, int initial_fbl_buffer_size,
                                                int total_loaded_leaves, int new_index,
                                                char SIMD_flag, int sample_size,
                                                int sample_type)
{
    int i;
    isax_index_settings *settings = malloc(sizeof(isax_index_settings));
    if(settings == NULL) {
        fprintf(stderr,"error: could not allocate memory for index settings.\n");
        return NULL;
    }
    
    if(new_index) {
		if(chdir(root_directory) == 0)
		{
			fprintf(stderr, "WARNING! Target index directory already exists. Please delete or choose a new one.\n");
		}
		
        settings->max_total_full_buffer_size = max_total_buffer_size;
        settings->initial_fbl_buffer_size = initial_fbl_buffer_size;
    }
    else {
    	if(chdir(root_directory) != 0)
		{
			fprintf(stderr, "WARNING! Target index directory does not exist!\n");
		}
    	else {
    		chdir("../");
    	}
        settings->max_total_full_buffer_size = max_total_buffer_size;
        settings->initial_fbl_buffer_size = initial_fbl_buffer_size;
        //settings->max_total_full_buffer_size = 0;
        //settings->initial_fbl_buffer_size = 0;
    }

    if(LSH_dimensionality > (int)(8 * (int)sizeof(root_mask_type))){
        fprintf(stderr,"error: Too many paa segments. The maximum value is %zu.\n", 
                8 * sizeof(root_mask_type));
        return NULL;
    }
    if(LSH_dimensionality>data_dimensionality)
    {
        fprintf(stderr,"error: Too many paa-segments. Using timeseries-size as paa-segments instead.\n");
        LSH_dimensionality = data_dimensionality;
    }
        
    if(initial_leaf_buffer_size < max_leaf_size)
    {
        fprintf(stderr,"error: Leaf buffers should be at least as big as leafs.\n");
        return NULL;
    }
    settings->total_loaded_leaves = total_loaded_leaves;
    settings->root_directory = root_directory;
    settings->raw_filename = NULL;

    settings->data_dimensionality = data_dimensionality;
    settings->LSH_dimensionality = LSH_dimensionality;
    settings->max_leaf_size = max_leaf_size;
    settings->min_leaf_size = min_leaf_size;
    settings->initial_leaf_buffer_size = initial_leaf_buffer_size;
	
    settings->sax_byte_size = (sizeof(sax_type) * LSH_dimensionality);
    settings->point_byte_size = (sizeof(data_type) * data_dimensionality);
    settings->position_byte_size = sizeof(file_position_type);
    
    settings->full_record_size = settings->sax_byte_size 
                                 + settings->position_byte_size 
                                 + settings->point_byte_size;
    settings->partial_record_size = settings->sax_byte_size 
                                    + settings->position_byte_size;
    
    settings->sax_bit_cardinality = sax_bit_cardinality;
    settings->sax_alphabet_cardinality = pow(2, sax_bit_cardinality);
	
	settings->max_sax_cardinalities = malloc(sizeof(sax_type) * settings->LSH_dimensionality);
	for(i=0; i<settings->LSH_dimensionality;i++)
		settings->max_sax_cardinalities[i] = settings->sax_bit_cardinality;
	
    //settings->mindist_sqrt = sqrtf((float) settings->data_dimensionality /
    //                               (float) settings->LSH_dimensionality);
    settings->mindist_sqrt = ((float) settings->data_dimensionality /
                                   (float) settings->LSH_dimensionality);
    settings->root_nodes_size = pow(2, settings->LSH_dimensionality);
    
    // SEGMENTS * (CARDINALITY)
    float c_size = ceil(log10(settings->sax_alphabet_cardinality + 1));
    settings->max_filename_size = settings->LSH_dimensionality * 
                                  ((c_size * 2) + 2)
                                  + 5 + strlen(root_directory);
    
    
    if(LSH_dimensionality > sax_bit_cardinality)
    {
        settings->bit_masks = malloc(sizeof(root_mask_type) * (LSH_dimensionality+1));
        if(settings->bit_masks == NULL) {
            fprintf(stderr,"error: could not allocate memory for bit masks.\n");
            return NULL;
        }
        
        for (; LSH_dimensionality>=0; LSH_dimensionality--)
        {
            settings->bit_masks[LSH_dimensionality] = pow(2, LSH_dimensionality);
        }
    }
    else
    {
        settings->bit_masks = malloc(sizeof(root_mask_type) * (sax_bit_cardinality+1));
        if(settings->bit_masks == NULL) {
            fprintf(stderr,"error: could not allocate memory for bit masks.\n");
            return NULL;
        }
        
        for (; sax_bit_cardinality>=0; sax_bit_cardinality--)
        {
            settings->bit_masks[sax_bit_cardinality] = pow(2, sax_bit_cardinality);
        }
    }
    
    if(new_index) {
    settings->max_total_buffer_size = (int) ((float)(settings->full_record_size/
                                       (float)settings->partial_record_size) * settings->max_total_full_buffer_size);
    }
    else {
    settings->max_total_buffer_size = settings->max_total_full_buffer_size;
    }
    
    if(SIMD_flag)   settings->SIMD_flag='1';
    else            settings->SIMD_flag=false; 

    settings->sample_size = sample_size;
    settings->sample_type = sample_type;

    return settings;
}


/**
 This function initializes an isax index
 @param isax_index_settings *settings
 @return isax_index
 */
isax_index * isax_index_init(isax_index_settings *settings)
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
    char *sax_filename = malloc((strlen(settings->root_directory) + 15) * sizeof(char));
    sax_filename = strcpy(sax_filename, settings->root_directory);
    sax_filename = strcat(sax_filename, "isax_file.sax");

    if( access( sax_filename, F_OK ) != -1 ) {
    	index->sax_file = fopen(sax_filename, "rb");
    } else {
    	index->sax_file = fopen(sax_filename, "w+b");
    }

    free(sax_filename);
    index->sax_cache = NULL;
    
    index->total_records = 0;
    index->loaded_records = 0;

    index->root_nodes = 0;
    index->allocated_memory = 0;
    index->has_wedges = 0;
    //index->locations = malloc(sizeof(int) * settings->data_dimensionality);

    index->answer = malloc(sizeof(data_type) * settings->data_dimensionality);

    return index;
}


/** 
 This function destroys an index.
 @param isax_index *index
 @param isax_ndoe *node
 */
void isax_index_destroy(isax_index *index, isax_node *node)
{
    if (node == NULL) {
        free(index->answer);
    	free(index->settings->bit_masks);
    	free(index->settings->raw_filename);
    	free(index->settings->max_sax_cardinalities);
    	free(index->settings);

		// TODO: OPTIMIZE TO FLUSH WITHOUT TRAVERSAL!
        isax_node *subtree_root = index->first_node;
        
        while (subtree_root != NULL) 
        {
            isax_node *next =  subtree_root->next;
            isax_index_destroy(index, subtree_root);
            subtree_root = next;
        }

        destroy_fbl(index->fbl);
        destroy_fbl(index->fbl);
		#ifdef CLUSTERED
			free(index->locations);
		#endif
        if (index->sax_file !=NULL)
        {
            fclose(index->sax_file);
        }
        if(index->sax_cache != NULL)
            free(index->sax_cache);
        free(index);
    }
    else {
        // Traverse tree
        if(!node->is_leaf) {
            if(node->right_child !=NULL)
                isax_index_destroy(index, node->right_child);
            if(node->left_child !=NULL)
            isax_index_destroy(index, node->left_child);
        }
        
        if(node->split_data != NULL)
        {
            free(node->split_data->split_mask);
            free(node->split_data);
        }
        if(node->filename != NULL)
        {
            free(node->filename);
        }
        if(node->isax_cardinalities != NULL)
        {
            free(node->isax_cardinalities);
        }
        if(node->isax_values != NULL)
        {
            free(node->isax_values);
        }
        if(node->buffer != NULL)
        {
            destroy_node_buffer(node->buffer);
        }
        free(node);
    }
}


enum response create_node_filename(isax_index *index,
                                   isax_node *node,
                                   isax_node_record *record)
{
    int i;

    node->filename = malloc(sizeof(char) * index->settings->max_filename_size);
    sprintf(node->filename, "%s", index->settings->root_directory);
    int l = (int) strlen(index->settings->root_directory);
    
    // If this has a parent then it is not a root node and as such it does have some 
    // split data on its parent about the cardinalities.
    node->isax_values = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
    node->isax_cardinalities = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
    
    if (node->parent) {
        for (i=0; i<index->settings->LSH_dimensionality; i++) {
            root_mask_type mask = 0x00;
            int k; 
            for (k=0; k <= node->parent->split_data->split_mask[i]; k++) 
            {
                mask |= (index->settings->bit_masks[index->settings->sax_bit_cardinality - 1 - k] & 
                         record->sax[i]);
            }
            mask = mask >> index->settings->sax_bit_cardinality - node->parent->split_data->split_mask[i] - 1;
            
            node->isax_values[i] = (int) mask;
            node->isax_cardinalities[i] = node->parent->split_data->split_mask[i]+1;
            
            if (i==0) {
                l += sprintf(node->filename+l ,"%d.%d", node->isax_values[i], node->isax_cardinalities[i]);
            }
            else {
                l += sprintf(node->filename+l ,"_%d.%d", node->isax_values[i], node->isax_cardinalities[i]);
            } 
            
        }
    }
    // If it has no parent it is root node and as such it's cardinality is 1.
    else
    {
        root_mask_type mask = 0x00;
        
        for (i=0; i<index->settings->LSH_dimensionality; i++) {
            
            mask = (index->settings->bit_masks[index->settings->sax_bit_cardinality - 1] & record->sax[i]);
            mask = mask >> index->settings->sax_bit_cardinality - 1;
            
            node->isax_values[i] = (int) mask;
            node->isax_cardinalities[i] = 1;
            
            if (i==0) {
                l += sprintf(node->filename+l ,"%d.1", (int) mask);
            }
            else {
                l += sprintf(node->filename+l ,"_%d.1", (int) mask);
            } 
        }
    }
    
#ifdef DEBUG
    printf("\tCreated filename:\t\t %s\n\n", node->filename);
#endif
    
    return SUCCESS;
}


isax_node * add_record_to_node(isax_index *index, 
                                 isax_node *tree_node, 
                                 isax_node_record *record,
                                 const char leaf_size_check) 
{
    #ifdef DEBUG
    printf("*** Adding to node ***\n\n");
    #endif
    isax_node *node = tree_node;

    // Traverse tree
    while (!node->is_leaf) {
        int location = index->settings->sax_bit_cardinality - 1 -
        node->split_data->split_mask[node->split_data->splitpoint];
        
        root_mask_type mask = index->settings->bit_masks[location];
        if(record->sax[node->split_data->splitpoint] & mask) 
        {
            node = node->right_child;
        }
        else
        {
            node = node->left_child;
        }
    }
    // Check if split needed
    if ((node->leaf_size) >= index->settings->max_leaf_size && leaf_size_check) {
    #ifdef DEBUG
        printf(">>> %s leaf size: %d\n\n", node->filename, node->leaf_size);
    #endif
        split_node(index, node);
        add_record_to_node(index, node, record, leaf_size_check);
    }
    else
    {
        if (node->filename == NULL) {
            create_node_filename(index,node,record);
        }
        add_to_node_buffer(node->buffer, record, index->settings->LSH_dimensionality, 
                           index->settings->data_dimensionality);
        node->leaf_size++;

    }
    return node;
}

/**
 Flushes all leaf buffers of a tree to disk.
 */
enum response flush_all_leaf_buffers(isax_index *index, enum buffer_cleaning_mode buffer_clean_mode)
{
#ifndef DEBUG
    //printf("\n");
    fflush(stdout);
#else
    printf("*** FLUSHING ***\n\n");
#endif
    // TODO: OPTIMIZE TO FLUSH WITHOUT TRAVERSAL!
    isax_node *subtree_root = index->first_node;
    
#ifndef DEBUG
#if VERBOSE_LEVEL == 2
    int i=1;
    fprintf(stdout, "\x1b[31mFlushing: \x1b[36m00.00%%\x1b[0m");
#endif
#if VERBOSE_LEVEL == 1
    fprintf(stdout, "Flushing...\n");
#endif
#endif
    
    while (subtree_root != NULL) 
    {
#ifndef DEBUG
#if VERBOSE_LEVEL == 2
        fprintf(stdout,"\r\x1b[31mFlushing: \x1b[36m%2.2lf%%\x1b[0m", ((float)i/(float)index->root_nodes)*100);
        i++;
        fflush(stdout);
#endif
#endif
        if(flush_subtree_leaf_buffers(index, subtree_root) == FAILURE)
            return FAILURE;                                              
        subtree_root = subtree_root->next;
    }
#ifndef DEBUG
#if VERBOSE_LEVEL == 2
    printf("\n");
#endif
#endif
    
    // *** FREE MEMORY OF NODES BUFFERS ***
    // Now that we have flushed its contents
    // there is no need to keep memory allocated.
    isax_index_clear_node_buffers(index, NULL, 1, buffer_clean_mode);
    
    return SUCCESS;
}


/**
 Flushes the leaf buffers of a sub-tree to disk.
 */
enum response flush_subtree_leaf_buffers (isax_index *index, isax_node *node)
{
    
    if (node->is_leaf && node->filename != NULL) {
        // Set that unloaded data exist in disk
        if (node->buffer->partial_buffer_size > 0 
            || node->buffer->tmp_partial_buffer_size > 0) {
            node->has_partial_data_file = 1;
        }
        // Set that the node has flushed full data in the disk
        if (node->buffer->full_buffer_size > 0 
            || node->buffer->tmp_full_buffer_size > 0) {
            node->has_full_data_file = 1;
        }

        if(node->has_full_data_file) {
            int prev_rec_count = node->leaf_size - (node->buffer->full_buffer_size + node->buffer->tmp_full_buffer_size);
            
            int previous_page_size =  ceil((float) (prev_rec_count * index->settings->full_record_size) / (float) PAGE_SIZE);   
            int current_page_size =   ceil((float) (node->leaf_size * index->settings->full_record_size) / (float) PAGE_SIZE);
            
            index->memory_info.disk_data_full += (current_page_size - previous_page_size);
        }
        if(node->has_partial_data_file) {
            int prev_rec_count = node->leaf_size - (node->buffer->partial_buffer_size + node->buffer->tmp_partial_buffer_size);
            
            int previous_page_size =  ceil((float) (prev_rec_count * index->settings->partial_record_size) / (float) PAGE_SIZE);   
            int current_page_size =   ceil((float) (node->leaf_size * index->settings->partial_record_size) / (float) PAGE_SIZE);
            
            index->memory_info.disk_data_partial += (current_page_size - previous_page_size);
        }
        if(node->has_full_data_file && node->has_partial_data_file) {
             printf("WARNING: (Mem size counting) this leaf has both partial and full data.\n");
        }
        index->memory_info.disk_data_full += (node->buffer->full_buffer_size + 
                                              node->buffer->tmp_full_buffer_size);

        index->memory_info.disk_data_partial += (node->buffer->partial_buffer_size + 
                                                 node->buffer->tmp_partial_buffer_size); 

        flush_node_buffer(node->buffer, index->settings->LSH_dimensionality, 
                          index->settings->data_dimensionality,
                          node->filename);
    }
    else if (!node->is_leaf)
    {
        flush_subtree_leaf_buffers(index, node->left_child);
        flush_subtree_leaf_buffers(index, node->right_child);
    }
    
    return SUCCESS;
}

void isax_index_clear_node_buffers(isax_index *index, isax_node *node, 
                                   enum node_cleaning_mode node_cleaning_mode,
                                   enum buffer_cleaning_mode buffer_clean_mode) 
{
    if (node == NULL) {
        // TODO: OPTIMIZE TO FLUSH WITHOUT TRAVERSAL!
        isax_node *subtree_root = index->first_node;
        
        while (subtree_root != NULL) 
        {
            
            isax_index_clear_node_buffers(index, subtree_root, node_cleaning_mode, buffer_clean_mode);
            subtree_root = subtree_root->next;
        }
    }
    else {
        // Traverse tree
                    //printf("this is the time 2\n");

        if(!node->is_leaf && node_cleaning_mode == INCLUDE_CHILDREN) {
            isax_index_clear_node_buffers(index, node->right_child, node_cleaning_mode, buffer_clean_mode);
            isax_index_clear_node_buffers(index, node->left_child, node_cleaning_mode, buffer_clean_mode);
        }
        else if(node->is_leaf && node->buffer != NULL)
        {
            clear_node_buffer(node->buffer, buffer_clean_mode);
        }
    }
}


int comp(const void * a, const void * b) 
{
    isax_node_record *ra = (isax_node_record*) a;
    isax_node_record *rb = (isax_node_record*) b;
    
    if (*ra->position==*rb->position)
        return 0;
    else
        if (*ra->position < *rb->position)
            return -1;
        else
            return 1;
}

void find_empty_children(isax_node *start_node, isax_node **nodes_to_load,
                         int * number, int * offset) {
    
    
    if (*number == 0 || start_node == NULL) {
        return;
    }
    //printf("*** Loading all children of node: %p\n", start_node);
    
    
    if (start_node->is_leaf && (start_node->has_partial_data_file || 
                                start_node->buffer->partial_buffer_size > 0 ||
                                start_node->buffer->tmp_partial_buffer_size > 0
                                )) {
        nodes_to_load[*offset] = start_node;
        (*offset)++;
        (*number)--;
#ifdef DEBUG
        printf("*** %pis a leaf, loaded.\n", start_node);
#endif
    }
    else {
        if (start_node->left_child != NULL) {
            find_empty_children(start_node->left_child, nodes_to_load, 
                                number, offset);
        }
        if(start_node->right_child != NULL) {
            find_empty_children(start_node->right_child, nodes_to_load, 
                                number, offset);
        }
    }
}



int find_siblings(isax_node *c_node, isax_node **nodes_to_load,
                  int *number_of_leaves, int *offset) {
    
    find_empty_children(c_node, nodes_to_load, number_of_leaves, offset);
    
    // Current node is already loaded.
    if (offset == 0) {
        return 0;
    }
    
    // In-subtree siblings...
    while (c_node->parent != NULL && *number_of_leaves > 0) {
        isax_node *parent = NULL;
        isax_node *sibling = NULL;
        
        parent = c_node->parent;
        if (parent->left_child == c_node) 
            sibling = parent->right_child;
        if (parent->right_child == c_node)
            sibling = parent->left_child;

#ifdef DEBUG
        printf("Found sibling: %p\n", sibling);
#endif
        find_empty_children(sibling, nodes_to_load, number_of_leaves, offset);
        
        c_node = c_node->parent;
    }
    
#ifndef CLUSTERED
    // In different sub-tree siblings... only if it is not clustered.
    isax_node *ptr_left = c_node->previous;
    isax_node *ptr_right = c_node->next;
    while (*number_of_leaves > 0 && 
           !(ptr_left == NULL && ptr_right == NULL)) {
        if(ptr_left != NULL)
        {
            find_empty_children(ptr_left, nodes_to_load, 
                                number_of_leaves, offset);
            ptr_left = ptr_left->previous;
            
        }
        if(ptr_right != NULL)
        {
            find_empty_children(ptr_right, nodes_to_load, 
                                number_of_leaves, offset);
            ptr_right = ptr_right->next;
        }
    }
#endif
    return SUCCESS;
}

float isax_index_load_node(isax_index *index, isax_node *c_node, data_type * query, float bsf) {
	isax_node *fbl_node = c_node;
    #ifdef CLUSTERED
    while (fbl_node->parent != NULL){
        fbl_node = fbl_node->parent;
    }
    char* pfilename = malloc(255);
    snprintf(pfilename, 255, "%s.%llu",index->settings->raw_filename, fbl_node->mask);
    FILE * raw_file = fopen(pfilename, "r");
    #else
    FILE * raw_file = fopen(index->settings->raw_filename, "r");
    #endif
    
    int total_loaded_leaves = index->settings->total_loaded_leaves;
    // *** Step 1 *** Decide nodes to load
    int leaves_to_load = total_loaded_leaves; 
    int offset = 0;
    isax_node **nodes_to_load = malloc(sizeof(isax_node *) * leaves_to_load);
    find_siblings(c_node, nodes_to_load, &leaves_to_load, &offset);
    
    // Current node is already loaded.
    if (offset == 0) {
        return FLT_MAX;
    }
    
    // *** Step 2 *** Prepare load buffers for all nodes
    isax_node_record *load_buffer = NULL;
    int total_records = 0;
    int node_id=0;
    int load_buffer_index = 0;
    
    for (node_id = 0; node_id < offset; node_id++) {
        isax_node * node = nodes_to_load[node_id];
            
        FILE * partial_file;
        int file_records = 0;
        int buffer_records = 0;
        
        if (node->buffer)
        {
            buffer_records = node->buffer->partial_buffer_size 
            + node->buffer->tmp_partial_buffer_size;
            total_records += buffer_records;
        }
        
        if(node->has_partial_data_file) {
        	char * partial_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
            strcpy(partial_fname, node->filename);
            strcat(partial_fname, ".part");
            COUNT_INPUT_TIME_START
            partial_file = fopen(partial_fname, "r");
            if(partial_file != NULL) {
                fseek(partial_file, 0L, SEEK_END);
                size_t file_size = ftell(partial_file);
                fseek(partial_file, 0L, SEEK_SET);
                file_records = (int) file_size / (int)index->settings->partial_record_size;
                total_records += file_records;
            }
            else {
            	fprintf(stderr, "[ERROR] LEAF FILE \"%s\" DOES NOT EXIST.\n", partial_fname);
            	system("ls ");
            }
            COUNT_INPUT_TIME_END
            free(partial_fname);
        }
         
        if (load_buffer == NULL)
            load_buffer = malloc(sizeof(isax_node_record) * (total_records));
        else
            load_buffer = realloc(load_buffer, sizeof(isax_node_record) * 
                                  (total_records));
        
        // PARTIAL DATA THAT ARE LOCATED IN BUFFERS
        if (node->buffer != NULL) {
            int i;
            for (i=0; i<node->buffer->partial_buffer_size; i++) {
                // CREATE RECORD AND SET AS NO_TMP
                // NO MALLOC JUST REFERENCE
                load_buffer[load_buffer_index].position = node->buffer->partial_position_buffer[i];
                load_buffer[load_buffer_index].sax = node->buffer->partial_sax_buffer[i];
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | NO_TMP;
                load_buffer[load_buffer_index].destination = node;
                load_buffer_index++;
            }
            node->buffer->partial_buffer_size = 0;
            
            for (i=0; i<node->buffer->tmp_partial_buffer_size; i++) {
                // CREATE RECORD AND SET AS TMP
                // NO MALLOC JUST REFERENCE
                load_buffer[load_buffer_index].position = node->buffer->tmp_partial_position_buffer[i];
                load_buffer[load_buffer_index].sax = node->buffer->tmp_partial_sax_buffer[i];
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | TMP;
                load_buffer[load_buffer_index].destination = node;
                load_buffer_index++;
            }
            node->buffer->tmp_partial_buffer_size = 0;
            // Count memory that was allocated here
            index->allocated_memory += index->settings->point_byte_size * (node->buffer->tmp_partial_buffer_size +
                                                                        node->buffer->partial_buffer_size);
        }
        
        // PARTIAL DATA THAT ARE IN FILE ON DISK
        if (file_records > 0) {
            int i;
            for (i=0; i<file_records; i++) {
                // CREATE RECORD AND SET AS TMP
                // MALLOC FOR SAX, TS, LOCATION
                load_buffer[load_buffer_index].position = malloc(index->settings->position_byte_size);
                load_buffer[load_buffer_index].sax = malloc(index->settings->sax_byte_size);
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | TMP;
                load_buffer[load_buffer_index].destination = node;
                COUNT_INPUT_TIME_START
                fread(load_buffer[load_buffer_index].position, sizeof(file_position_type), 1, partial_file);
                fread(load_buffer[load_buffer_index].sax, sizeof(sax_type), index->settings->LSH_dimensionality, partial_file);
                COUNT_INPUT_TIME_END
                load_buffer_index++;
            }


            // Count memory that was allocated here
            index->allocated_memory += (file_records * index->settings->full_record_size);
            
            // Remove memory that is deallocated from disk
            index->memory_info.disk_data_partial -= ceil((float)(file_records * index->settings->partial_record_size) / (float) PAGE_SIZE);
            
            COUNT_INPUT_TIME_START
            fclose(partial_file);
            COUNT_INPUT_TIME_END
        }
        #ifdef BENCHMARK
        if (total_records > 0) {
            COUNT_LOADED_NODE()
        }
        #endif
        
        load_buffer_index = total_records;
    }
    
    
    // *** Step 3 *** Sort array of data to load
    qsort(load_buffer,total_records,sizeof(isax_node_record), comp);
    
    
    // *** Step 4 *** Read node data from raw file and store in tree
    //float bsf = FLT_MAX;
    int i;
    for (i=0; i<total_records; i++) {
        COUNT_LOADED_RECORD()
        index->loaded_records++;
        
        //printf("LOADING POSITION: %d\n", *load_buffer[i].position);
        COUNT_INPUT_TIME_START
        fseek(raw_file, *load_buffer[i].position, SEEK_SET);
        fread(load_buffer[i].ts, sizeof(data_type), index->settings->data_dimensionality, raw_file);
        COUNT_INPUT_TIME_END
        

        if(query != NULL)
        {
            // Also calculate distance
            float dist = ts_euclidean_distance(query, load_buffer[i].ts, 
                                               index->settings->data_dimensionality, bsf);
            if (dist < bsf) {
                bsf = dist;
                #ifdef STORE_ANSWER
                data_type *ts = load_buffer[i].ts;
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }
        }
        
        //printf("Distance: %lf\n", dist);
        
        add_to_node_buffer(((isax_node *)load_buffer[i].destination)->buffer, 
                           &load_buffer[i], 
                           index->settings->LSH_dimensionality, 
                           index->settings->data_dimensionality);
    }
    free(load_buffer);
    COUNT_INPUT_TIME_START
    fclose(raw_file);
    COUNT_INPUT_TIME_END
    
    // Set nodes as loaded.
    for (node_id = 0; node_id < offset; node_id++) {
        nodes_to_load[node_id]->has_partial_data_file = 0;
    }
    
    if (index->allocated_memory > index->settings->max_total_full_buffer_size)
    {
        FLUSHES++;
        flush_all_leaf_buffers(index, TMP_ONLY_CLEAN);
        index->allocated_memory = 0;
    }
    free(nodes_to_load);
    return bsf;
}
void isax_index_load_node_topk(isax_index *index, isax_node *c_node, data_type * query, pqueue_bsf *pq_bsf) {
    isax_node *fbl_node = c_node;
    #ifdef CLUSTERED
    while (fbl_node->parent != NULL){
        fbl_node = fbl_node->parent;
    }
    char* pfilename = malloc(255);
    snprintf(pfilename, 255, "%s.%llu",index->settings->raw_filename, fbl_node->mask);
    FILE * raw_file = fopen(pfilename, "r");
    #else
    FILE * raw_file = fopen(index->settings->raw_filename, "r");
    #endif
    
    int total_loaded_leaves = index->settings->total_loaded_leaves;
    // *** Step 1 *** Decide nodes to load
    int leaves_to_load = total_loaded_leaves; 
    int offset = 0;
    isax_node **nodes_to_load = malloc(sizeof(isax_node *) * leaves_to_load);
    find_siblings(c_node, nodes_to_load, &leaves_to_load, &offset);
    
    // Current node is already loaded.
    if (offset == 0) {
        return;
    }
    
    // *** Step 2 *** Prepare load buffers for all nodes
    isax_node_record *load_buffer = NULL;
    int total_records = 0;
    int node_id=0;
    int load_buffer_index = 0;
    
    for (node_id = 0; node_id < offset; node_id++) {
        isax_node * node = nodes_to_load[node_id];
            
        FILE * partial_file;
        int file_records = 0;
        int buffer_records = 0;
        
        if (node->buffer)
        {
            buffer_records = node->buffer->partial_buffer_size 
            + node->buffer->tmp_partial_buffer_size;
            total_records += buffer_records;
        }
        
        if(node->has_partial_data_file) {
            char * partial_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
            strcpy(partial_fname, node->filename);
            strcat(partial_fname, ".part");
            COUNT_INPUT_TIME_START
            partial_file = fopen(partial_fname, "r");
            if(partial_file != NULL) {
                fseek(partial_file, 0L, SEEK_END);
                size_t file_size = ftell(partial_file);
                fseek(partial_file, 0L, SEEK_SET);
                file_records = (int) file_size / (int)index->settings->partial_record_size;
                total_records += file_records;
            }
            else {
                fprintf(stderr, "[ERROR] LEAF FILE \"%s\" DOES NOT EXIST.\n", partial_fname);
                system("ls ");
            }
            COUNT_INPUT_TIME_END
            free(partial_fname);
        }
         
        if (load_buffer == NULL)
            load_buffer = malloc(sizeof(isax_node_record) * (total_records));
        else
            load_buffer = realloc(load_buffer, sizeof(isax_node_record) * 
                                  (total_records));
        
        // PARTIAL DATA THAT ARE LOCATED IN BUFFERS
        if (node->buffer != NULL) {
            int i;
            for (i=0; i<node->buffer->partial_buffer_size; i++) {
                // CREATE RECORD AND SET AS NO_TMP
                // NO MALLOC JUST REFERENCE
                load_buffer[load_buffer_index].position = node->buffer->partial_position_buffer[i];
                load_buffer[load_buffer_index].sax = node->buffer->partial_sax_buffer[i];
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | NO_TMP;
                load_buffer[load_buffer_index].destination = node;
                load_buffer_index++;
            }
            node->buffer->partial_buffer_size = 0;
            
            for (i=0; i<node->buffer->tmp_partial_buffer_size; i++) {
                // CREATE RECORD AND SET AS TMP
                // NO MALLOC JUST REFERENCE
                load_buffer[load_buffer_index].position = node->buffer->tmp_partial_position_buffer[i];
                load_buffer[load_buffer_index].sax = node->buffer->tmp_partial_sax_buffer[i];
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | TMP;
                load_buffer[load_buffer_index].destination = node;
                load_buffer_index++;
            }
            node->buffer->tmp_partial_buffer_size = 0;
            // Count memory that was allocated here
            index->allocated_memory += index->settings->point_byte_size * (node->buffer->tmp_partial_buffer_size +
                                                                        node->buffer->partial_buffer_size);
        }
        
        // PARTIAL DATA THAT ARE IN FILE ON DISK
        if (file_records > 0) {
            int i;
            for (i=0; i<file_records; i++) {
                // CREATE RECORD AND SET AS TMP
                // MALLOC FOR SAX, TS, LOCATION
                load_buffer[load_buffer_index].position = malloc(index->settings->position_byte_size);
                load_buffer[load_buffer_index].sax = malloc(index->settings->sax_byte_size);
                load_buffer[load_buffer_index].ts = malloc(index->settings->point_byte_size);
                load_buffer[load_buffer_index].insertion_mode = FULL | TMP;
                load_buffer[load_buffer_index].destination = node;
                COUNT_INPUT_TIME_START
                fread(load_buffer[load_buffer_index].position, sizeof(file_position_type), 1, partial_file);
                fread(load_buffer[load_buffer_index].sax, sizeof(sax_type), index->settings->LSH_dimensionality, partial_file);
                COUNT_INPUT_TIME_END
                load_buffer_index++;
            }


            // Count memory that was allocated here
            index->allocated_memory += (file_records * index->settings->full_record_size);
            
            // Remove memory that is deallocated from disk
            index->memory_info.disk_data_partial -= ceil((float)(file_records * index->settings->partial_record_size) / (float) PAGE_SIZE);
            
            COUNT_INPUT_TIME_START
            fclose(partial_file);
            COUNT_INPUT_TIME_END
        }
        #ifdef BENCHMARK
        if (total_records > 0) {
            COUNT_LOADED_NODE()
        }
        #endif
        
        load_buffer_index = total_records;
    }
    
    
    // *** Step 3 *** Sort array of data to load
    qsort(load_buffer,total_records,sizeof(isax_node_record), comp);
    
    
    // *** Step 4 *** Read node data from raw file and store in tree
    //float bsf = FLT_MAX;
    int i;
    for (i=0; i<total_records; i++) {
        COUNT_LOADED_RECORD()
        index->loaded_records++;
        
        //printf("LOADING POSITION: %d\n", *load_buffer[i].position);
        COUNT_INPUT_TIME_START
        fseek(raw_file, *load_buffer[i].position, SEEK_SET);
        fread(load_buffer[i].ts, sizeof(data_type), index->settings->data_dimensionality, raw_file);
        COUNT_INPUT_TIME_END
        

        if(query != NULL)
        {
            // Also calculate distance
            float dist = ts_euclidean_distance(query, load_buffer[i].ts, 
                                               index->settings->data_dimensionality, pq_bsf->knn[pq_bsf->k-1]);
            if (dist <= pq_bsf->knn[pq_bsf->k-1]) {
        pqueue_bsf_insert(pq_bsf,dist,0,NULL);
                #ifdef STORE_ANSWER
                data_type *ts = load_buffer[i].ts;
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }

        }
        
        //printf("Distance: %lf\n", dist);
        
        add_to_node_buffer(((isax_node *)load_buffer[i].destination)->buffer, 
                           &load_buffer[i], 
                           index->settings->LSH_dimensionality, 
                           index->settings->data_dimensionality);
    }
    free(load_buffer);
    COUNT_INPUT_TIME_START
    fclose(raw_file);
    COUNT_INPUT_TIME_END
    
    // Set nodes as loaded.
    for (node_id = 0; node_id < offset; node_id++) {
        nodes_to_load[node_id]->has_partial_data_file = 0;
    }
    
    if (index->allocated_memory > index->settings->max_total_full_buffer_size)
    {
        FLUSHES++;
        flush_all_leaf_buffers(index, TMP_ONLY_CLEAN);
        index->allocated_memory = 0;
    }
    free(nodes_to_load);
}
float calculate_minimum_distance (isax_index *index, isax_node *node, data_type *raw_query, data_type *query) 
{
	//printf("Calculating minimum distance...\n");
	float bsfLeaf =   minidist_paa_to_isax(query, node->isax_values, 
                                                                     node->isax_cardinalities,
                                                                     index->settings->sax_bit_cardinality,  
                                                                     index->settings->sax_alphabet_cardinality, 
                                                                     index->settings->LSH_dimensionality, 
                                                                     MINVAL, MAXVAL,
                                                                     index->settings->mindist_sqrt);
	float bsfRecord = FLT_MAX;																 
	//printf("---> Distance: %lf\n", bsfLeaf);
    //sax_print(node->isax_values, 1,  index->settings->sax_bit_cardinality);

	if(!index->has_wedges) {
    //		printf("--------------\n");
		int i = 0;
		if(node->has_partial_data_file) {
			char * partial_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
			strcpy(partial_fname, node->filename);
			strcat(partial_fname, ".part");
			COUNT_INPUT_TIME_START
			FILE * partial_file = fopen(partial_fname, "r");
			sax_type *sax = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
			file_position_type *pos = malloc(sizeof(file_position_type));
			while(!feof(partial_file)) {
				if(fread(pos, sizeof(file_position_type), 1, partial_file)) {
					if(fread(sax, sizeof(sax_type), index->settings->LSH_dimensionality, partial_file)) {
						float mindist = minidist_paa_to_isax_raw(query, sax, index->settings->max_sax_cardinalities,
															 index->settings->sax_bit_cardinality,
															 index->settings->sax_alphabet_cardinality,
															 index->settings->LSH_dimensionality, MINVAL, MAXVAL,
															 index->settings->mindist_sqrt);
    //			printf("+[FILE] %lf\n", mindist);

						if(mindist < bsfRecord) {
							bsfRecord = mindist;
						}
					}
				}
			}
			fclose(partial_file);
			COUNT_INPUT_TIME_END
			free(sax);
			free(pos);
			free(partial_fname);
		}

		if (node->buffer != NULL) {
			for (i=0; i<node->buffer->partial_buffer_size; i++) {
				float mindist = minidist_paa_to_isax_raw(query, node->buffer->partial_sax_buffer[i], index->settings->max_sax_cardinalities,
													 index->settings->sax_bit_cardinality,
													 index->settings->sax_alphabet_cardinality,
													 index->settings->LSH_dimensionality, MINVAL, MAXVAL,
													 index->settings->mindist_sqrt);
    //				printf("+[PARTIAL] %lf\n", mindist);
				if(mindist < bsfRecord) {
					bsfRecord = mindist;
				}
			}

			for (i=0; i<node->buffer->tmp_partial_buffer_size; i++) {
				float mindist = minidist_paa_to_isax_raw(query, node->buffer->tmp_partial_sax_buffer[i], index->settings->max_sax_cardinalities,
													 index->settings->sax_bit_cardinality,
													 index->settings->sax_alphabet_cardinality,
													 index->settings->LSH_dimensionality, MINVAL, MAXVAL,
													 index->settings->mindist_sqrt);
    //				printf("+[TMP_PARTIAL] %lf\n", mindist);
				if(mindist < bsfRecord) {
					bsfRecord = mindist;
				}
			}
		}
	}
	else {
		int i=0;
		if(node->wedges[0] == FLT_MIN) {
			bsfRecord = FLT_MAX;
		}
		else {
			bsfRecord = 0;
			data_type *min_wedge = &node->wedges[0];
			data_type *max_wedge = &node->wedges[index->settings->data_dimensionality];
			if(raw_query[i] > max_wedge[i]) {
				bsfRecord += (raw_query[i] - max_wedge[i]) * (raw_query[i] - max_wedge[i]);
			}
			else if(raw_query[i] < max_wedge[i] && raw_query[i] > min_wedge[i]) {
				//bound += 0;
			}
			else {
				bsfRecord += (min_wedge[i] - raw_query[i]) * (min_wedge[i] - raw_query[i]);
			}
			//bsfRecord = sqrtf(bsfRecord);
		}

	}
	float bsf = (bsfRecord == FLT_MAX) ? bsfLeaf : bsfRecord;
    //	printf("\t%.2lf - %d [%d] : %s.%s\n",bsfRecord, node->leaf_size, node->is_leaf, node->filename, node->has_full_data_file ? ".full" : ".part");

		
	//printf("---> Final: %lf\n", bsf);
	return  bsf;
}
float calculate_minimum_distance_SIMD (isax_index *index, isax_node *node, data_type *raw_query, data_type *query) 
{
    //printf("Calculating minimum distance...\n");
    float bsfLeaf =   minidist_paa_to_isax(query, node->isax_values, 
                                                                     node->isax_cardinalities,
                                                                     index->settings->sax_bit_cardinality,  
                                                                     index->settings->sax_alphabet_cardinality, 
                                                                     index->settings->LSH_dimensionality, 
                                                                     MINVAL, MAXVAL,
                                                                     index->settings->mindist_sqrt);
    float bsfRecord = FLT_MAX;                                                               
    //printf("---> Distance: %lf\n", bsfLeaf);
    //sax_print(node->isax_values, 1,  index->settings->sax_bit_cardinality);

    if(!index->has_wedges) {
    //      printf("--------------\n");
        int i = 0;
        if(node->has_partial_data_file) {
            char * partial_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
            strcpy(partial_fname, node->filename);
            strcat(partial_fname, ".part");
            COUNT_INPUT_TIME_START
            FILE * partial_file = fopen(partial_fname, "r");
            sax_type *sax = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
            file_position_type *pos = malloc(sizeof(file_position_type));
            while(!feof(partial_file)) {
                if(fread(pos, sizeof(file_position_type), 1, partial_file)) {
                    if(fread(sax, sizeof(sax_type), index->settings->LSH_dimensionality, partial_file)) {
                        float mindist = minidist_paa_to_isax_raw_SIMD(query, sax, index->settings->max_sax_cardinalities,
                                                             index->settings->sax_bit_cardinality,
                                                             index->settings->sax_alphabet_cardinality,
                                                             index->settings->LSH_dimensionality, MINVAL, MAXVAL,
                                                             index->settings->mindist_sqrt);
    //          printf("+[FILE] %lf\n", mindist);

                        if(mindist < bsfRecord) {
                            bsfRecord = mindist;
                        }
                    }
                }
            }
            fclose(partial_file);
            COUNT_INPUT_TIME_END
            free(sax);
            free(pos);
            free(partial_fname);
        }

        if (node->buffer != NULL) {
            for (i=0; i<node->buffer->partial_buffer_size; i++) {
                float mindist = minidist_paa_to_isax_raw_SIMD(query, node->buffer->partial_sax_buffer[i], index->settings->max_sax_cardinalities,
                                                     index->settings->sax_bit_cardinality,
                                                     index->settings->sax_alphabet_cardinality,
                                                     index->settings->LSH_dimensionality, MINVAL, MAXVAL,
                                                     index->settings->mindist_sqrt);
    //              printf("+[PARTIAL] %lf\n", mindist);
                if(mindist < bsfRecord) {
                    bsfRecord = mindist;
                }
            }

            for (i=0; i<node->buffer->tmp_partial_buffer_size; i++) {
                float mindist = minidist_paa_to_isax_raw_SIMD(query, node->buffer->tmp_partial_sax_buffer[i], index->settings->max_sax_cardinalities,
                                                     index->settings->sax_bit_cardinality,
                                                     index->settings->sax_alphabet_cardinality,
                                                     index->settings->LSH_dimensionality, MINVAL, MAXVAL,
                                                     index->settings->mindist_sqrt);
    //              printf("+[TMP_PARTIAL] %lf\n", mindist);
                if(mindist < bsfRecord) {
                    bsfRecord = mindist;
                }
            }
        }
    }
    else {
        int i=0;
        if(node->wedges[0] == FLT_MIN) {
            bsfRecord = FLT_MAX;
        }
        else {
            bsfRecord = 0;
            data_type *min_wedge = &node->wedges[0];
            data_type *max_wedge = &node->wedges[index->settings->data_dimensionality];
            if(raw_query[i] > max_wedge[i]) {
                bsfRecord += (raw_query[i] - max_wedge[i]) * (raw_query[i] - max_wedge[i]);
            }
            else if(raw_query[i] < max_wedge[i] && raw_query[i] > min_wedge[i]) {
                //bound += 0;
            }
            else {
                bsfRecord += (min_wedge[i] - raw_query[i]) * (min_wedge[i] - raw_query[i]);
            }
            //bsfRecord = sqrtf(bsfRecord);
        }

    }
    float bsf = (bsfRecord == FLT_MAX) ? bsfLeaf : bsfRecord;
    //  printf("\t%.2lf - %d [%d] : %s.%s\n",bsfRecord, node->leaf_size, node->is_leaf, node->filename, node->has_full_data_file ? ".full" : ".part");

        
    //printf("---> Final: %lf\n", bsf);
    return  bsf;
}
float calculate_node_distance (isax_index *index, isax_node *node, data_type *query, float bsf) 
{
    COUNT_CHECKED_NODE()

    // If node has buffered data
    if (node->buffer != NULL) 
    {
        int i;

        for (i=0; i<node->buffer->full_buffer_size; i++) {
            float dist = ts_euclidean_distance(query, node->buffer->full_ts_buffer[i], 
                                               index->settings->data_dimensionality, bsf);
            if (dist < bsf) {
                bsf = dist;
                #ifdef STORE_ANSWER
                data_type *ts = node->buffer->full_ts_buffer[i];
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }
        }
        for (i=0; i<node->buffer->tmp_full_buffer_size; i++) {
            float dist = ts_euclidean_distance(query, node->buffer->tmp_full_ts_buffer[i], 
                                               index->settings->data_dimensionality, bsf);
            if (dist < bsf) {
                bsf = dist;
                #ifdef STORE_ANSWER
                data_type *ts = node->buffer->tmp_full_ts_buffer[i];
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }
        }

    }
    
    if (node->has_full_data_file) 
    {
    	char * full_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
        strcpy(full_fname, node->filename);
        strcat(full_fname, ".full");
        COUNT_INPUT_TIME_START
        FILE * full_file = fopen(full_fname, "r");
        COUNT_INPUT_TIME_END
        
        if(full_file != NULL) {
            COUNT_INPUT_TIME_START
            fseek(full_file, 0L, SEEK_END);
            size_t file_size = ftell(full_file);
            fseek(full_file, 0L, SEEK_SET);
            COUNT_INPUT_TIME_END
            int file_records = (int) file_size / (int)index->settings->full_record_size;
            data_type *ts = malloc(index->settings->point_byte_size);
            while (file_records > 0) {

                COUNT_INPUT_TIME_START
                fseek(full_file, sizeof(file_position_type), SEEK_CUR);
                fseek(full_file, index->settings->sax_byte_size, SEEK_CUR);
                fread(ts, sizeof(data_type), index->settings->data_dimensionality, full_file);
                COUNT_INPUT_TIME_END

                float dist = ts_euclidean_distance(query, ts,
                                                   index->settings->data_dimensionality, bsf);
                //if(conter_ts<30)
                //{printf(" %f \n", dist);
                //conter_ts++;
            //}
                if (dist < bsf) {
                    bsf = dist;
                    #ifdef STORE_ANSWER
                    memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                    #endif
                }
                file_records--;
            } 
            
            free(ts);
        }

        COUNT_INPUT_TIME_START
        fclose(full_file);
        COUNT_INPUT_TIME_END
        free(full_fname);
    }
    

    //////// ADAPTIVE ////////////////////
    
    if (node->is_leaf && (node->has_partial_data_file || 
                    node->buffer->partial_buffer_size > 0 ||
                    node->buffer->tmp_partial_buffer_size > 0
                    )) {

    	float partial_bsf = isax_index_load_node(index, node, query, bsf);
        
        if (partial_bsf < bsf)
            bsf = partial_bsf;
    }
    
    //////////////////////////////////////
    
    return bsf;
}

float calculate_node_distance_SIMD (isax_index *index, isax_node *node, data_type *query, float bsf) 
{
    COUNT_CHECKED_NODE()
    
    // If node has buffered data

    if (node->buffer != NULL) 
    {
        int i;

        for (i=0; i<node->buffer->full_buffer_size; i++) {
            float dist = ts_euclidean_distance_SIMD(query, node->buffer->full_ts_buffer[i], 
                                               index->settings->data_dimensionality, bsf);
            if (dist < bsf) {
                bsf = dist;
                #ifdef STORE_ANSWER
                data_type *ts = node->buffer->full_ts_buffer[i];
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }
        }
        for (i=0; i<node->buffer->tmp_full_buffer_size; i++) {
            float dist = ts_euclidean_distance_SIMD(query, node->buffer->tmp_full_ts_buffer[i], 
                                               index->settings->data_dimensionality, bsf);
            if (dist < bsf) {
                bsf = dist;
                #ifdef STORE_ANSWER
                data_type *ts = node->buffer->tmp_full_ts_buffer[i];
                memcpy(index->answer, ts, index->settings->data_dimensionality * sizeof(data_type));
                #endif
            }
        }
    }
    
    if (node->has_full_data_file) 
    {
        char * full_fname = malloc(sizeof(char) * (strlen(node->filename) + 6));
        strcpy(full_fname, node->filename);
        strcat(full_fname, ".full");
        COUNT_INPUT_TIME_START
        FILE * full_file = fopen(full_fname, "r");
        COUNT_INPUT_TIME_END
        
        if(full_file != NULL) {
            COUNT_INPUT_TIME_START
            fseek(full_file, 0L, SEEK_END);
            size_t file_size = ftell(full_file);
            fseek(full_file, 0L, SEEK_SET);
            COUNT_INPUT_TIME_END
            int file_records = (int) file_size / (int)index->settings->full_record_size;
            int i=0;
            data_type *tsp;
            void *ts_buffer = malloc(file_size);
            //fseek(full_file, sizeof(file_position_type), SEEK_CUR);
            //fseek(full_file, index->settings->sax_byte_size, SEEK_CUR);
                            COUNT_INPUT_TIME_START

            fread(ts_buffer, file_size,1, full_file);
                            COUNT_INPUT_TIME_END

            //printf("wefwaeweaaef is%ld\n",index->settings->full_record_size );
            while (file_records > 0) {

                COUNT_INPUT_TIME_START
                //fseek(full_file, sizeof(file_position_type), SEEK_CUR);
                //fseek(full_file, index->settings->sax_byte_size, SEEK_CUR);
                //fread(ts, sizeof(data_type), index->settings->data_dimensionality, full_file);
                COUNT_INPUT_TIME_END
                tsp=ts_buffer+i*index->settings->full_record_size+sizeof(file_position_type)+index->settings->sax_byte_size;
                float dist = ts_euclidean_distance_SIMD(query, tsp,
                                                   index->settings->data_dimensionality, bsf);
                //if(conter_ts<30)
                //{printf(" %f \n", dist);
                //conter_ts++;
            //}
                if (dist < bsf) {
                    bsf = dist;
                    #ifdef STORE_ANSWER
                    //memcpy(index->answer, &(ts_buffer[i]), index->settings->data_dimensionality * sizeof(data_type));
                    #endif
                }
                file_records--;
                i++;
            } 
            
            //free(ts);
            free(ts_buffer);
        }

        COUNT_INPUT_TIME_START
        fclose(full_file);
        COUNT_INPUT_TIME_END
        free(full_fname);
    }
    

    //////// ADAPTIVE ////////////////////
    
    if (node->is_leaf && (node->has_partial_data_file || 
                    node->buffer->partial_buffer_size > 0 ||
                    node->buffer->tmp_partial_buffer_size > 0
                    )) {
        //printf("check point 1!!!!!\n");
        float partial_bsf = isax_index_load_node(index, node, query, bsf);
        
        if (partial_bsf < bsf)
            bsf = partial_bsf;
    }
    
    //////////////////////////////////////
    
    return bsf;
}
void load_random_leaf(isax_index *index) {
    isax_node *rnode = index->first_node;
    
    while (rnode->next != NULL) {
        rnode = rnode->next;
        isax_node *curr = rnode;
        while (!curr->is_leaf) {
            curr = curr->left_child;
        }
        isax_index_load_node(index, curr, NULL, FLT_MAX);
    }
}

void complete_index_leafs(isax_index *index) 
{
    isax_node *subtree_root = index->first_node;
    while (subtree_root != NULL) {
        complete_subtree_leafs(index, subtree_root);
        subtree_root = subtree_root->next;
    }
    // TODO: CHECK THAT THIS IS NEEDED!
    FLUSHES++;
    flush_all_leaf_buffers(index, TMP_ONLY_CLEAN);
    index->allocated_memory = 0;
}

void complete_subtree_leafs(isax_index *index, isax_node *node) 
{
    if(node->is_leaf)
    {
        isax_index_load_node(index, node, NULL, FLT_MAX);
    }
    else 
    {
        complete_subtree_leafs(index, node->left_child);
        complete_subtree_leafs(index, node->right_child);
    }
}

void cache_sax_file(isax_index *index) {
    COUNT_OUTPUT_TIME_START
	fflush(index->sax_file);
	COUNT_OUTPUT_TIME_END
	fseek(index->sax_file, 0L, SEEK_END);
	unsigned long size = ftell(index->sax_file);
	fseek(index->sax_file, 0L, SEEK_SET);
	index->sax_cache = malloc(size);
	index->sax_cache_size = size / index->settings->sax_byte_size;
	COUNT_INPUT_TIME_START
	fread(index->sax_cache, index->settings->sax_byte_size, index->sax_cache_size, index->sax_file);
	COUNT_INPUT_TIME_END

    index->memory_info.mem_summaries = index->sax_cache_size;
}

void node_write(isax_index *index, isax_node *node, FILE *file) {
	fwrite(&(node->is_leaf), sizeof(unsigned char), 1, file);
	fwrite(&(node->mask), sizeof(root_mask_type), 1, file);

	if(node->is_leaf) {
		if(node->filename != NULL) {
			int filename_size = strlen(node->filename);
			fwrite(&filename_size, sizeof(int), 1, file);
			fwrite(node->filename, sizeof(char), filename_size, file);
			fwrite(&(node->leaf_size), sizeof(int), 1, file);
			fwrite(&(node->has_full_data_file), sizeof(char), 1, file);
			fwrite(&(node->has_partial_data_file), sizeof(char), 1, file);
		}
		else {
			int filename_size = 0;
			fwrite(&filename_size, sizeof(int), 1, file);
		}
	}
	else {
		fwrite(&(node->split_data->splitpoint), sizeof(int), 1, file);
		fwrite(node->split_data->split_mask, sizeof(sax_type), index->settings->LSH_dimensionality, file);
	}

	if(node->isax_cardinalities != NULL) {
		char has_isax_data = 1;
		fwrite(&has_isax_data, sizeof(char), 1, file);
		fwrite(node->isax_cardinalities, sizeof(sax_type), index->settings->LSH_dimensionality, file);
		fwrite(node->isax_values, sizeof(sax_type), index->settings->LSH_dimensionality, file);
	}
	else {
		char has_isax_data = 0;
		fwrite(&has_isax_data, sizeof(char), 1, file);
	}

	if(!node->is_leaf) {
		node_write(index, node->left_child, file);
		node_write(index, node->right_child, file);
	}
}

isax_node *node_read(isax_index *index, FILE *file) {
	isax_node *node;

	char is_leaf = 0;
	fread(&is_leaf, sizeof(unsigned char), 1, file);

	root_mask_type mask = 0;
	fread(&(mask), sizeof(root_mask_type), 1, file);

    index->memory_info.mem_tree_structure++;

	if(is_leaf) {
		node = malloc(sizeof(isax_node));
		node->is_leaf = 1;
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
		node->buffer = init_node_buffer(index->settings->initial_leaf_buffer_size);
		node->mask = 0;

		int filename_size = 0;
		fread(&filename_size, sizeof(int), 1, file);
		if(filename_size > 0) {
			node->filename = malloc(sizeof(char) * (filename_size + 1));
			fread(node->filename, sizeof(char), filename_size, file);
			node->filename[filename_size] = '\0';
			fread(&(node->leaf_size), sizeof(int), 1, file);
			fread(&(node->has_full_data_file), sizeof(char), 1, file);
			fread(&(node->has_partial_data_file), sizeof(char), 1, file);
			COUNT_NEW_NODE();

            if(node->has_full_data_file) {
                float number_of_bytes = (float) (node->leaf_size * index->settings->full_record_size);
                int number_of_pages = ceil(number_of_bytes / (float) PAGE_SIZE);
                index->memory_info.disk_data_full += number_of_pages;
            }
            else if(node->has_partial_data_file) {
                float number_of_bytes = (float) (node->leaf_size * index->settings->partial_record_size);
                int number_of_pages = ceil(number_of_bytes / (float) PAGE_SIZE);
                index->memory_info.disk_data_partial += number_of_pages;
            }
            if(node->has_full_data_file && node->has_partial_data_file) {
                printf("WARNING: (Mem size counting) this leaf has both partial and full data.\n");
            }
		}
		else {
			node->filename = NULL;
			node->leaf_size = 0;
			node->has_full_data_file = 0;
			node->has_partial_data_file = 0;
		}
	}
	else {
		node = malloc(sizeof(isax_node));
		node->buffer = NULL;
		node->is_leaf = 0;
		node->filename = NULL;
		node->has_full_data_file = 0;
		node->has_partial_data_file = 0;
		node->leaf_size = 0;
		node->split_data = malloc(sizeof(isax_node_split_data));
		node->split_data->split_mask = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);

		fread(&(node->split_data->splitpoint), sizeof(int), 1, file);
		fread(node->split_data->split_mask, sizeof(sax_type), index->settings->LSH_dimensionality, file);
	}
	node->mask = mask;

	char has_isax_data = 0;
	fread(&has_isax_data, sizeof(char), 1, file);

	if(has_isax_data) {
		node->isax_cardinalities = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
		node->isax_values = malloc(sizeof(sax_type) * index->settings->LSH_dimensionality);
		fread(node->isax_cardinalities, sizeof(sax_type), index->settings->LSH_dimensionality, file);
		fread(node->isax_values, sizeof(sax_type), index->settings->LSH_dimensionality, file);
	}
	else {
		node->isax_cardinalities = NULL;
		node->isax_values = NULL;
	}

	if(!is_leaf) {
		node->left_child = node_read(index, file);
		node->right_child = node_read(index, file);
	}

	return node;
}


void index_write(isax_index *index)
{
	fprintf(stderr, ">>> Storing index: %s\n", index->settings->root_directory);
	char *filename = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
	filename = strcpy(filename, index->settings->root_directory);
	filename = strcat(filename, "/index.idx");
	FILE *file = fopen(filename, "wb");
	free(filename);

	int raw_filename_size = strlen(index->settings->raw_filename);
	int data_dimensionality = index->settings->data_dimensionality;
	int LSH_dimensionality = index->settings->LSH_dimensionality;
	int sax_bit_cardinality = index->settings->sax_bit_cardinality;
	int max_leaf_size = index->settings->max_leaf_size;
	int min_leaf_size = index->settings->min_leaf_size;
	int initial_leaf_buffer_size = index->settings->initial_leaf_buffer_size;
	int max_total_buffer_size = index->settings->max_total_buffer_size;
	int initial_fbl_buffer_size = index->settings->initial_fbl_buffer_size;
	int total_loaded_leaves = index->settings->total_loaded_leaves;
	int new_index = 0;
	// SETTINGS DATA
	fwrite(&raw_filename_size, sizeof(int), 1, file);
	fwrite(index->settings->raw_filename, sizeof(char), raw_filename_size, file);
	fwrite(&data_dimensionality, sizeof(int), 1, file);
	fwrite(&LSH_dimensionality, sizeof(int), 1, file);
	fwrite(&sax_bit_cardinality, sizeof(int), 1, file);
	fwrite(&max_leaf_size, sizeof(int), 1, file);
	fwrite(&min_leaf_size, sizeof(int), 1, file);
	fwrite(&initial_leaf_buffer_size, sizeof(int), 1, file);
	fwrite(&max_total_buffer_size, sizeof(int), 1, file);
	fwrite(&initial_fbl_buffer_size, sizeof(int), 1, file);
	fwrite(&total_loaded_leaves, sizeof(int), 1, file);
	// FBL DATA AND NODES
	int j;
	for (j=0; j<index->fbl->number_of_buffers; j++) {
		fbl_soft_buffer *current_fbl_node = &index->fbl->soft_buffers[j];
		if (current_fbl_node->initialized && current_fbl_node->node != NULL) {
			fwrite(&j, sizeof(int), 1, file);
			node_write(index, current_fbl_node->node, file);
		}
	}
	fclose(file);

    char *filename_adpt = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    filename_adpt = strcpy(filename_adpt, index->settings->root_directory);
    filename_adpt = strcat(filename_adpt, "/adaptive");
    FILE *file_adpt = fopen(filename_adpt, "wb");
    free(filename_adpt);
    fwrite(&(index->total_records), sizeof(unsigned long long), 1, file_adpt);
    fwrite(&(index->loaded_records), sizeof(unsigned long long), 1, file_adpt);
    fclose(file_adpt);    

}
void index_mRecBuf_write(isax_index *index)
{
    fprintf(stderr, ">>> Storing index: %s\n", index->settings->root_directory);
    char *filename = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    
    filename = strcpy(filename, index->settings->root_directory);
    //filename = strcat(filename, "/index.idx");
    filename = strcat(filename, "_index.idx");
    fprintf(stderr, "FILENAME %s", filename);
    FILE *file = fopen(filename, "wb");
    free(filename);

    int raw_filename_size = strlen(index->settings->raw_filename);
    int data_dimensionality = index->settings->data_dimensionality;
    int LSH_dimensionality = index->settings->LSH_dimensionality;
    int sax_bit_cardinality = index->settings->sax_bit_cardinality;
    int max_leaf_size = index->settings->max_leaf_size;
    int min_leaf_size = index->settings->min_leaf_size;
    int initial_leaf_buffer_size = index->settings->initial_leaf_buffer_size;
    int max_total_buffer_size = index->settings->max_total_buffer_size;
    int initial_fbl_buffer_size = index->settings->initial_fbl_buffer_size;
    int total_loaded_leaves = index->settings->total_loaded_leaves;
    int new_index = 0;
    // SETTINGS DATA
    fwrite(&raw_filename_size, sizeof(int), 1, file);
    fwrite(index->settings->raw_filename, sizeof(char), raw_filename_size, file);
    fwrite(&data_dimensionality, sizeof(int), 1, file);
    fwrite(&LSH_dimensionality, sizeof(int), 1, file);
    fwrite(&sax_bit_cardinality, sizeof(int), 1, file);
    fwrite(&max_leaf_size, sizeof(int), 1, file);
    fwrite(&min_leaf_size, sizeof(int), 1, file);
    fwrite(&initial_leaf_buffer_size, sizeof(int), 1, file);
    fwrite(&max_total_buffer_size, sizeof(int), 1, file);
    fwrite(&initial_fbl_buffer_size, sizeof(int), 1, file);
    fwrite(&total_loaded_leaves, sizeof(int), 1, file);

    int sample_size = index->settings->sample_size;
    int sample_type = index->settings->sample_type;

    fwrite(&sample_size, sizeof(int), 1, file);
    fwrite(&sample_type, sizeof(int), 1, file);

    for(int i=0; i<index->settings->LSH_dimensionality; ++i)
    {
        fwrite(index->bins[i], sizeof(float), index->settings->sax_alphabet_cardinality-1, file);
        /*
        for(int j=0; j<index->settings->sax_alphabet_cardinality-1; ++j)
        {
            float binning_value = index->bins[i][j];
            fwrite(&binning_value, sizeof(float), 1, file);
        }*/
    }

    // FBL DATA AND NODES
    int j;
    for (j=0; j<index->fbl->number_of_buffers; j++) {
        parallel_fbl_soft_buffer *current_fbl_node = &((parallel_first_buffer_layer*)(index->fbl))->soft_buffers[j];
        if (current_fbl_node->initialized && current_fbl_node->node != NULL) {
            fwrite(&j, sizeof(int), 1, file);
            node_write(index, current_fbl_node->node, file);
        }
    }
    fclose(file);

    char *filename_adpt = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    
    filename_adpt = strcpy(filename_adpt, index->settings->root_directory);
    //filename_adpt = strcat(filename_adpt, "/adaptive");
    filename_adpt = strcat(filename_adpt, "_adaptive");
    FILE *file_adpt = fopen(filename_adpt, "wb");
    free(filename_adpt);
    fwrite(&(index->total_records), sizeof(unsigned long long), 1, file_adpt);
    fwrite(&(index->loaded_records), sizeof(unsigned long long), 1, file_adpt);
    fclose(file_adpt);    

}
isax_index * index_read(const char* root_directory) {
	fprintf(stderr, ">>> Loading index: %s\n", root_directory);
	char *filename = malloc(sizeof(char) * (strlen(root_directory) + 15));
	filename = strcpy(filename, root_directory);
	filename = strcat(filename, "/index.idx");
	FILE *file = fopen(filename, "rb");
	free(filename);

	int raw_filename_size = 0;
	char *raw_filename = NULL;
	int data_dimensionality = 0;
	int LSH_dimensionality = 0;
	int sax_bit_cardinality = 0;
	int max_leaf_size = 0;
	int min_leaf_size = 0;
	int initial_leaf_buffer_size = 0;
	int max_total_buffer_size = 0;
	int initial_fbl_buffer_size = 0;
	int total_loaded_leaves = 0;
	int new_index = 0;

	fread(&raw_filename_size, sizeof(int), 1, file);
	raw_filename = malloc(sizeof(char) * (raw_filename_size+1));
    fread(raw_filename, sizeof(char), raw_filename_size, file);
	raw_filename[raw_filename_size] = '\0';
    
    fread(&data_dimensionality, sizeof(int), 1, file);
	fread(&LSH_dimensionality, sizeof(int), 1, file);
	fread(&sax_bit_cardinality, sizeof(int), 1, file);
	fread(&max_leaf_size, sizeof(int), 1, file);
	fread(&min_leaf_size, sizeof(int), 1, file);
	fread(&initial_leaf_buffer_size, sizeof(int), 1, file);
	fread(&max_total_buffer_size, sizeof(int), 1, file);
	fread(&initial_fbl_buffer_size, sizeof(int), 1, file);
	fread(&total_loaded_leaves, sizeof(int), 1, file);

	isax_index_settings *idx_settings = isax_index_settings_init(root_directory,
																data_dimensionality,
																LSH_dimensionality,
																sax_bit_cardinality,
																max_leaf_size,
																min_leaf_size,
																initial_leaf_buffer_size,
																max_total_buffer_size,
																initial_fbl_buffer_size,
																total_loaded_leaves,
																0,false,1,1);
	idx_settings->raw_filename = malloc(sizeof(char) * 256);
	strcpy(idx_settings->raw_filename, raw_filename);
	free(raw_filename);

	isax_index *index = isax_index_init(idx_settings);

	while(!feof(file)) {
		int j = 0;
		if(fread(&j, sizeof(int), 1, file)) {
			fbl_soft_buffer *current_buffer = &index->fbl->soft_buffers[j];
			current_buffer->initialized = 1;
			current_buffer->node = node_read(index, file);
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
	}

	fclose(file);

    char *filename_adpt = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    filename_adpt = strcpy(filename_adpt, index->settings->root_directory);
    filename_adpt = strcat(filename_adpt, "/adaptive");
    FILE *file_adpt = fopen(filename_adpt, "rb");
    free(filename_adpt);

    if(file_adpt) {
        fread(&index->total_records, sizeof(unsigned long long), 1, file_adpt);
        fread(&index->loaded_records, sizeof(unsigned long long), 1, file_adpt);
        fclose(file_adpt);       
    }
    
	return index;
}

void get_index_size(isax_index *index, struct stat *stat_index, struct stat *stat_adaptive)
{
    char *filename_index = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    filename_index = strcpy(filename_index, index->settings->root_directory);
    filename_index = strcat(filename_index, "_index.idx");

    char *filename_adpt = malloc(sizeof(char) * (strlen(index->settings->root_directory) + 15));
    filename_adpt = strcpy(filename_adpt, index->settings->root_directory);
    filename_adpt = strcat(filename_adpt, "_adaptive");

    if(stat(filename_index, stat_index)==0 && stat(filename_adpt, stat_adaptive)==0)
    {
        return;
    }
    else
    {
        fprintf(stderr, "ERROR: unable to find stat for file %s and %s", filename_index, filename_adpt);
        return;
    }
}

void create_wedges(isax_index *index, isax_node *node) {
	if(node == NULL) {
		int j;
		for (j=0; j<index->fbl->number_of_buffers; j++) {
			fbl_soft_buffer *current_fbl_node = &index->fbl->soft_buffers[j];
			if (current_fbl_node->initialized && current_fbl_node->node != NULL) {
				isax_node *curr_node = current_fbl_node->node;
				create_wedges(index, curr_node);
			}
		}
		index->has_wedges = 1;
	}
	else {
		if(node->is_leaf) {
			node->wedges = malloc(sizeof(data_type) * index->settings->data_dimensionality * 2);
			int i;
			for(i=0; i<index->settings->data_dimensionality; i++) {
				node->wedges[i] = MINVAL;
				node->wedges[index->settings->data_dimensionality + i] = MAXVAL;
			}
			if(node->has_partial_data_file) {
				char *filename;
				file_position_type *positions;
				data_type *tmp_ts = malloc(sizeof(data_type) * index->settings->data_dimensionality);

				filename = malloc((strlen(node->filename) + 10) * sizeof(char));
				strcpy(filename, node->filename);
				strcat(filename, ".part");
				filename[strlen(node->filename)+5] = '\0';

				FILE *leaf_file = fopen(filename, "rb");
				FILE *raw_file = fopen(index->settings->raw_filename, "rb");

				fseeko(leaf_file,0L, SEEK_END);
				unsigned long size = ftello(leaf_file) / index->settings->partial_record_size;
				fseeko(leaf_file, 0L, SEEK_SET);

				positions = malloc(index->settings->position_byte_size * size);

				for(i=0; i<size; i++) {
					fread(&positions[i], index->settings->position_byte_size, 1, leaf_file);
					fseeko(leaf_file, index->settings->sax_byte_size, SEEK_CUR);
				}

				qsort(positions, size, index->settings->position_byte_size, compare_file_positions);

				for(i=0; i<size; i++) {
					fseek(raw_file, positions[i] * index->settings->point_byte_size, SEEK_SET);
					fread(tmp_ts, index->settings->point_byte_size, 1, raw_file);

					int j;
					for(j=0; j<index->settings->data_dimensionality; j++) {
						if(tmp_ts[j] < node->wedges[index->settings->data_dimensionality + j]) {
							node->wedges[index->settings->data_dimensionality + j] = tmp_ts[j];
						}
						if(tmp_ts[j] > node->wedges[j]) {
							node->wedges[j] = tmp_ts[j];
						}
					}
				}

				fclose(raw_file);
				fclose(leaf_file);
				free(filename);
				free(positions);
				free(tmp_ts);
			}
			if(node->has_full_data_file) {

				char *filename;

				filename = malloc((strlen(node->filename) + 10) * sizeof(char));
				strcpy(filename, node->filename);
				strcat(filename, ".full");
				filename[strlen(node->filename)+5] = '\0';
				FILE *leaf_file = fopen(filename, "rb");

				fseeko(leaf_file,0L, SEEK_END);
				unsigned long size = ftello(leaf_file) / index->settings->full_record_size;
				fseeko(leaf_file, 0L, SEEK_SET);

				for(i=0; i<size; i++) {
					fseeko(leaf_file, index->settings->sax_byte_size + index->settings->position_byte_size, SEEK_CUR);
					int j=0;
					for(j=0; j<index->settings->data_dimensionality; j++) {
						data_type tmp_val;
						fread(&tmp_val, sizeof(data_type), 1, leaf_file);
						if(tmp_val < node->wedges[index->settings->data_dimensionality + j]) {
							node->wedges[index->settings->data_dimensionality + j] = tmp_val;
						}
						if(tmp_val > node->wedges[j]) {
							node->wedges[j] = tmp_val;
						}
					}
				}



				///// PRINT
				/*char s[256];
				sprintf(s, "%p.WEDGE.txt", node);
				FILE *op = fopen(s, "w");

				for(i=0; i<index->settings->data_dimensionality; i++) {
					fprintf(op, "%lf ", node->wedges[i]);
				}
				fprintf(op, "\n");
				fseeko(leaf_file, 0L, SEEK_SET);
				for(i=0; i<size; i++) {
					fseeko(leaf_file, index->settings->sax_byte_size + index->settings->position_byte_size, SEEK_CUR);
					fread(tmp_ts, index->settings->point_byte_size, 1, leaf_file);
					int j;
					for(j=0; j<index->settings->data_dimensionality; j++) {
						fprintf(op, "%lf ", tmp_ts[j]);
					}
					fprintf(op, "\n");
				}
				for(i=0; i<index->settings->data_dimensionality; i++) {
					fprintf(op, "%lf ", node->wedges[index->settings->data_dimensionality + i]);
				}

				fprintf(op, "\n");
				fclose(op);*/
				///// END PRINT


				fclose(leaf_file);
				free(filename);
			}
		}
		else {
			create_wedges(index, node->left_child);
			create_wedges(index, node->right_child);
		}
	}
}

int compare_file_positions (const void * a, const void * b)
{
   return ( *(file_position_type*)a - *(file_position_type*)b );
}

void clear_wedges(isax_index *index, isax_node *node) {
	if(node == NULL) {
		int j;
		for (j=0; j<index->fbl->number_of_buffers; j++) {
			fbl_soft_buffer *current_fbl_node = &index->fbl->soft_buffers[j];
			if (current_fbl_node->initialized && current_fbl_node->node != NULL) {
				isax_node *curr_node = current_fbl_node->node;
				clear_wedges(index, curr_node);
			}
		}
		index->has_wedges = 0;
	}
	else {
		if(node->is_leaf) {
			free(node->wedges);
			node->wedges = NULL;
		}
		else {
			clear_wedges(index, node->left_child);
			clear_wedges(index, node->right_child);
		}
	}
}

meminfo get_memory_utilization_info(isax_index *index) {
    meminfo bytes_mem_info;
    memcpy(&bytes_mem_info, 
           &(index->memory_info), sizeof(meminfo));

    bytes_mem_info.mem_data = index->allocated_memory;
    bytes_mem_info.mem_tree_structure *= sizeof(isax_node);
    bytes_mem_info.mem_summaries *= (index->settings->LSH_dimensionality * sizeof(sax_type));
    bytes_mem_info.disk_data_full *= PAGE_SIZE;
    bytes_mem_info.disk_data_partial *= PAGE_SIZE;

    return bytes_mem_info;
}

void print_mem_info(isax_index *index) {
    meminfo bytes_mem_info = get_memory_utilization_info(index);
    printf("######################### MEMORY INFO ##########################\n");
    printf("# (MEM)     TREE STRUCTURE:     %ld\n",    bytes_mem_info.mem_tree_structure);
    printf("# (MEM)     SUMMARIZED DATA:    %ld\n",    bytes_mem_info.mem_summaries);
    printf("# (MEM)     RAW DATA:           %ld\n",    bytes_mem_info.mem_data);
    printf("# (DISK)    MATERIALIZED:       %ld\n",    bytes_mem_info.disk_data_full);
    printf("# (DISK)    PARTIAL:            %ld\n",    bytes_mem_info.disk_data_partial);
    printf("################################################################\n"); 
}




