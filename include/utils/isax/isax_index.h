#ifndef isaxlib_isax_index_h
#define isaxlib_isax_index_h
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>

#include "../../../config.h"
#include "../../../globals.h"
#include "isax_node.h"
#include "isax_node_record.h"
#include "../sax/ts.h"
#include "pqueue.h"
typedef struct {
    unsigned long mem_tree_structure;
    unsigned long mem_data; 
    unsigned long mem_summaries;
    unsigned long disk_data_full;
    unsigned long disk_data_partial;
} meminfo;


typedef struct {
    char new_index;

    char * raw_filename;
    const char* root_directory;
    int initial_fbl_buffer_size;
	sax_type *max_sax_cardinalities;
    
    int data_dimensionality;
    int LSH_dimensionality;
    
    int sax_byte_size;
    int position_byte_size;
    int point_byte_size;
    
    int full_record_size;
    int partial_record_size;
    
    // ALWAYS: SAX_ALPHABET_CARDINALITY = 2^SAX_BIT_CARDINALITY
    int sax_bit_cardinality;
    root_mask_type * bit_masks;
    int sax_alphabet_cardinality;
    
    int max_leaf_size;
    int min_leaf_size;
    int initial_leaf_buffer_size;
    int max_total_buffer_size;
    int max_total_full_buffer_size;
    
    int max_filename_size;
    
    float mindist_sqrt;
    int root_nodes_size;
    
    int total_loaded_leaves;

    char SIMD_flag;
 
    unsigned int sample_size;
    int sample_type;

} isax_index_settings;

typedef struct {
    meminfo memory_info;

    FILE *sax_file;
    sax_type *sax_cache;
    // data_type *lsh_cache;
    unsigned long sax_cache_size;

    unsigned long long allocated_memory;
    unsigned long root_nodes;
    unsigned long long total_records;
    unsigned long long loaded_records;
    
    int * locations;
    struct isax_node *first_node;
    isax_index_settings *settings;
    
    char has_wedges;

    struct first_buffer_layer *fbl;

    data_type *answer;

    data_type **bins;
    data_type *bins_lower_bound;
    data_type *bins_upper_bound;
    data_type norm_factor;

    pqueue_bsf *result;

    pqueue_result *range_queue_result;

    data_type ** lsh_hash_set;

    data_type ** lsh_mem_array;
    
    data_type ** lsh_mem_array_for_init;

    data_type ** lsh_mem_array_for_query;

    data_type ** query_points;
} isax_index;



//TODO: Put sanity check for variables (cardinalities etc.)

isax_index * isax_index_init(isax_index_settings *settings);
isax_index_settings * isax_index_settings_init (const char * root_directory,
                                                int data_dimensionality, 
                                                int LSH_dimensionality, 
                                                int sax_bit_cardinality,
                                                int max_leaf_size,
                                                int min_leaf_size,
                                                int initial_leaf_buffer_size,
                                                int max_total_buffer_size, 
                                                int initial_fbl_buffer_size,
                                                int total_loaded_leaves,
												int new_index,
                                                char SIMD_flag,
                                                int sample_size,
                                                int sample_type);

isax_node * add_record_to_node(isax_index *index, isax_node *node, 
                                 isax_node_record *record,
                                 const char leaf_size_check);
enum response flush_subtree_leaf_buffers (isax_index *index, isax_node *node);
enum response flush_all_leaf_buffers(isax_index *index, enum buffer_cleaning_mode buffer_clean_mode);
enum response create_node_filename(isax_index *index,
                                   isax_node *node,
                                   isax_node_record *record);
void isax_index_clear_node_buffers(isax_index *index, isax_node *node, 
                                   enum node_cleaning_mode node_cleaning_mode,
                                   enum buffer_cleaning_mode buffer_clean_mode);
void isax_index_destroy(isax_index *index, isax_node *node);

int comp(const void * a, const void * b);
void load_random_leaf(isax_index *index);

float calculate_node_distance (isax_index *index, isax_node *node, data_type *query, float bsf);
float calculate_node_distance_SIMD (isax_index *index, isax_node *node, data_type *query, float bsf);
float isax_index_load_node(isax_index *index, isax_node *c_node, data_type * query, float bsf);
void isax_index_load_node_topk(isax_index *index, isax_node *c_node, data_type * query, pqueue_bsf *pq_bsf);

void complete_index_leafs(isax_index *index);
void complete_subtree_leafs(isax_index *index, isax_node *node);

int find_siblings(isax_node *c_node, isax_node **nodes_to_load, 
                  int *number_of_leaves, int *offset);
float calculate_minimum_distance (isax_index *index, isax_node *node, data_type *raw_query, data_type *query);
float calculate_minimum_distance_SIMD (isax_index *index, isax_node *node, data_type *raw_query, data_type *query);
void cache_sax_file(isax_index *index);



void index_write(isax_index *index);
void index_mRecBuf_write(isax_index *index);
void node_write(isax_index *index, isax_node *node, FILE *file);
isax_index * index_read(const char * root_directory);
void get_index_size(isax_index *index, struct stat *stat_index, struct stat *stat_adaptive);
isax_node * node_read(isax_index *index, FILE *file);

//// WEDGES FUNCTIONALITY ////
void create_wedges(isax_index *index, isax_node *node);
void clear_wedges(isax_index *index, isax_node *node);
int compare_file_positions (const void * a, const void * b);
void print_mem_info(isax_index *index);
meminfo get_memory_utilization_info(isax_index *index);
#endif
