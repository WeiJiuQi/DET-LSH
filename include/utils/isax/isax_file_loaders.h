#ifndef isax_isax_file_loaders_h
#define isax_isax_file_loaders_h
#include "../../../config.h"
#include "../../../globals.h"
#include "../sax/ts.h"
#include "../sax/sax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "isax_index.h"
#include "../indexing_support.h"

void range_query(const char *ifilename, int q_num, isax_index *index,
                            float minimum_distance, int min_checked_leaves,int k, data_type** lsh_hash_set, float search_radius,
                            pqueue_t ** (*search_function)(data_type*, data_type*, isax_index*,node_list*, float, int, float));

#endif
