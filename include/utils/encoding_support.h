#ifndef encoding_support_h
#define encoding_support_h

#include "../../config.h"
#include "../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include "query_support.h"

#include <sys/types.h>

enum response breakpoints_init(isax_index *index);

void free_sampled_data(isax_index *index, data_type **sampled_data);

void query_to_node_dist(isax_index *index, float *lsh, sax_type *sax, sax_type *sax_cardinalities, query_result *dist_result);

long random_at_most(long max);

enum response encoding(isax_index *index, sax_type *sax_out, data_type *lsh);

typedef struct bins_data_inmemory
{
	isax_index *index;
	long int start_number,stop_number;	
	data_type ** sampled_data;
    const char *filename;
	int workernumber;
	long int records;
	long int records_offset;
	data_type * data_point;
	data_type * raw_file;
	data_type * transform;
	data_type ** lsh_hash_set;
}bins_data_inmemory;


#endif
