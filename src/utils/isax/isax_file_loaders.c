#include "../../../config.h"
#include "../../../globals.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <float.h>
#include <unistd.h>
#include <math.h>

#include "utils/isax/isax_node.h"
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_node_record.h"
#include "utils/isax/isax_file_loaders.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/indexing_support.h"
#include "utils/sax/ts.h"

void range_query(const char *ifilename, int q_num, isax_index *index,
                            float minimum_distance, int min_checked_leaves,int k,data_type** lsh_hash_set, float search_radius,
                            pqueue_t ** (*search_function)(data_type*, data_type*, isax_index*,node_list*, float, int, float)) 
{
    fprintf(stderr, ">>> Performing range queries in file: %s\n", ifilename);

    int q_loaded = 0;
    node_list nodelist;
    nodelist.nlist=malloc(sizeof(isax_node*)*pow(2, index->settings->LSH_dimensionality));
    nodelist.node_amount=0;
    isax_node *current_root_node = index->first_node;
    while(1)
    {
        if (current_root_node!=NULL)
        {
            nodelist.nlist[nodelist.node_amount]=current_root_node;
            current_root_node=current_root_node->next;
            nodelist.node_amount++;
        }
        else
        {
            break;
        }               
    }

    while (q_loaded < q_num)
    {
        index->range_queue_result[q_loaded].q_num = q_loaded;
        index->range_queue_result[q_loaded].pq = search_function(index->query_points[q_loaded], index->lsh_mem_array_for_query[q_loaded], index,&nodelist, minimum_distance, min_checked_leaves, search_radius);

        fflush(stdout);

        q_loaded++;
    }

    fprintf(stderr, ">>> Finished range queries.\n");
}

