#include "../../config.h"
#include "../../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "immintrin.h"
#ifdef VALUES
#include <values.h>
#endif

#include <sys/stat.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "utils/isax/isax_index.h"

#include "utils/encoding_support.h"

enum response breakpoints_init(isax_index *index)
{
    int data_dimensionality = index->settings->data_dimensionality;
    int num_symbols = index->settings->sax_alphabet_cardinality;
    int LSH_dimensionality = index->settings->LSH_dimensionality;

    index->bins = NULL;  
    index->bins = (data_type**) calloc(LSH_dimensionality,sizeof(data_type*));
    index->bins_lower_bound = (data_type*) calloc(LSH_dimensionality,sizeof(data_type));
    index->bins_upper_bound = (data_type*) calloc(LSH_dimensionality,sizeof(data_type));

    if(index == NULL) {
        fprintf(stderr,"Could not allocate memory for bins structure.\n");
        return FAILURE;
    }

    for (int i = 0; i < LSH_dimensionality; ++i)
    {
        index->bins[i] = calloc(num_symbols-1,sizeof(data_type));
        if(index == NULL) {
            fprintf(stderr,"Could not allocate memory for bins structure.\n");
            return FAILURE;
        }
        for (int j = 0; j < num_symbols-1; ++j)
        {
            index->bins[i][j] = FLT_MAX;
        }
    }

    fprintf(stderr,"Initialized breakpoints[%d][%d] \n", LSH_dimensionality, num_symbols-1 );

    return SUCCESS;
}

/*
    This function calculates random numbers between 0 and max
*/
long random_at_most(long max) {
	unsigned long
	num_bins = (unsigned long) max + 1,
	num_rand = (unsigned long) RAND_MAX + 1,
	bin_size = num_rand / num_bins,
	defect   = num_rand % num_bins;

	long x;
	do{
		x = random();
	}
	while (num_rand - defect <= (unsigned long)x);

	return x/bin_size;
}

void free_sampled_data(isax_index *index, data_type **sampled_data)
{
    int LSH_dimensionality = index->settings->LSH_dimensionality;
    for (int k = 0; k < LSH_dimensionality; ++k)
    {
        free(sampled_data[k]);
    }        
    free(sampled_data);
}

void query_to_node_dist(isax_index *index, float *lsh, sax_type *sax, sax_type *sax_cardinalities, query_result *dist_result)
{
    int min_val = MINVAL;
    int max_val = MAXVAL;

    sax_type max_bit_cardinality = index->settings->sax_bit_cardinality;
    int max_cardinality = index->settings->sax_alphabet_cardinality;
    int number_of_segments = index->settings->LSH_dimensionality;
   
    data_type upper_distance = 0.0;
    data_type lower_distance = 0.0;
    data_type lower_value, upper_value;

    for (int i=0; i<number_of_segments; i++) {

        sax_type c_c = sax_cardinalities[i];

        sax_type c_m = max_bit_cardinality;
        sax_type v = sax[i];
        
        sax_type region_lower = (v <<  (c_m - c_c));
        sax_type region_upper = (~((int)MAXFLOAT << (c_m - c_c)) | region_lower);

        float breakpoint_lower = 0;
        float breakpoint_upper = 0;
        
        
        if (region_lower == 0) {
            breakpoint_lower = index->bins_lower_bound[i];
        }
        else
        {
            breakpoint_lower = index->bins[i][ region_lower - 1];
        }
        
        if (region_upper == max_cardinality - 1) {
            breakpoint_upper = index->bins_upper_bound[i];
        }
        else
        {
            breakpoint_upper = index->bins[i][ region_upper];
        }
        
        if (breakpoint_lower > lsh[i]) {
            lower_value = breakpoint_lower - lsh[i];
            upper_value = breakpoint_upper - lsh[i];
            lower_distance += lower_value*lower_value;
            upper_distance += upper_value*upper_value;
        }
        else if(breakpoint_upper < lsh[i]) {
            lower_value = lsh[i] - breakpoint_upper;
            upper_value = lsh[i] - breakpoint_lower;
            lower_distance += lower_value*lower_value;
            upper_distance += upper_value*upper_value;
        }
    }

    dist_result->lower_distance = lower_distance;
    dist_result->upper_distance = upper_distance;
    
}

enum response encoding(isax_index *index, sax_type *sax_out, data_type *lsh)
{
    int LSH_dimensionality = index->settings->LSH_dimensionality;

    // binarysearch
    for (int k = 0; k < LSH_dimensionality; ++k)
    {
        int left = 0, right = index->settings->sax_alphabet_cardinality - 2;
        int middle;
        while (left <= right) {
            middle = (left + right) / 2;
            if (lsh[k] >= index->bins[k][middle]) {
                left = middle + 1;
            } else {
                right = middle - 1;
            }
        }
        sax_out[k] = (unsigned char) (left);
    }

    if(sax_out != NULL) return SUCCESS;
    else
    {
        fprintf(stderr, "encoding error");
    }
}