#include "../../../config.h"
#include "../../../globals.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "immintrin.h"
#include "utils/sax/ts.h"

/**
 This function converts a string of floats seperated by a delimeter into a ts 
 record of a size ts_size.
 @param char ts_str[]
 @param int ts_size
 @param const char * delims
 @return *ts
 */
void ts_parse_str(char ts_str[], data_type * ts_out, int ts_size, const char * delims)
{
    int index=0;
    char *result = strtok( ts_str, delims );
	while( result != NULL ) {
		ts_out[index] = atof(result);
		result = strtok( NULL, delims );
#ifdef SANITY_CHECK
        if (index >= ts_size)
        {
            fprintf(stderr, "sanity error: Time series bigger than limit of %d", ts_size);
            exit(-1); 
        }
#endif
        index++;
	}
    free(result);
}

float ts_euclidean_distance(data_type * t, data_type * s, int size, float bound) {
    float distance = 0;
    while (size > 0 && distance <bound) {
        size--;
        distance += (t[size] - s[size]) * (t[size] - s[size]);

    }

//    distance = sqrtf(distance);
    
    return distance;
}
float ts_euclidean_distance_SIMD(data_type * t, data_type * s, int size, float bound) {
    float distance = 0;
    int i =0;
    float distancef[8];

__m256 v_t,v_s,v_d,distancev;
    while (size > 0 && distance <bound) {
        v_t=_mm256_loadu_ps (&t[i]);
        v_s=_mm256_loadu_ps (&s[i]);
        
        v_d= _mm256_sub_ps (v_t,v_s);

        v_d=_mm256_mul_ps (v_d,v_d);
        size-=8;

        i=i+8;
        distancev = _mm256_hadd_ps (v_d, v_d);
        distancev = _mm256_hadd_ps (distancev, distancev);
        _mm256_storeu_ps (distancef ,distancev);
        distance +=distancef[0]+distancef[4];

    }

//    distance = sqrtf(distance);
    
    return distance;
}
float ts_euclidean_distance_neSIMD(data_type * t, data_type * s, int size) {
    float distance = 0;
    int i =0;
    float distancef[8];

    __m256 v_fd,v_t,v_s,v_d,distancev;

    v_fd=_mm256_setzero_ps ();
    while (size > 0) {
        v_t=_mm256_loadu_ps (&t[i]);
        v_s=_mm256_loadu_ps (&s[i]);
        
        v_d= _mm256_sub_ps (v_t,v_s);

        v_fd=_mm256_add_ps (v_fd,_mm256_mul_ps (v_d,v_d));
        size-=8;

        i=i+8;


    }

//    distance = sqrtf(distance);
    distancev = _mm256_hadd_ps (v_fd, v_fd);
    distancev = _mm256_hadd_ps (distancev, distancev);
    _mm256_storeu_ps (distancef ,distancev);
    distance +=distancef[0]+distancef[4];
    return distance;
}

/** 
 This function prints a ts record of a size.
 @param ts *ts
 @param int size
*/
void ts_print(data_type *ts, int size) 
{
    int i;
    for (i=0; i < size; i++) {
        printf("%lf", ts[i]);
    }
    printf("\n");
}
