#ifndef isaxlib_sax_h
#define isaxlib_sax_h
#include "../../../config.h"
#include "../../../globals.h"
#include "ts.h"

enum response sax_from_ts(data_type *ts_in, sax_type *sax_out, int ts_values_per_segment, 
            int segments, int cardinality, int bit_cardinality);
enum response sax_from_ts_new(data_type *ts_in, sax_type *sax_out, int ts_values_per_segment, 
            int segments, int cardinality, int bit_cardinality);
enum response sax_from_ts_SIMD(data_type *ts_in, sax_type *sax_out, int ts_values_per_segment, 
            int segments, int cardinality, int bit_cardinality);
void sax_print(sax_type *sax, int segments, int cardinality);
void printbin(unsigned long long n, int size);
void serial_printbin (unsigned long long n, int size);
int compare(const void *a, const void *b);
float minidist_paa_to_isax(float *paa, sax_type *sax, sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments, 
                           int min_val, int max_val, float ratio_sqrt);

float   minidist_paa_to_isax_SIMD(float *paa, sax_type *sax, 
                           sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           float ratio_sqrt);
float minidist_paa_to_isax_raw(float *paa, sax_type *sax, sax_type *sax_cardinalities,
						                              sax_type max_bit_cardinality,
						                              int max_cardinality,
						                              int number_of_segments, 
						                              int min_val, int max_val, float ratio_sqrt);
float   minidist_paa_to_isax_raw_SIMD(float *paa, sax_type *sax, 
                           sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           float ratio_sqrt);
float   minidist_paa_to_isax_rawa_SIMD(float *paa, sax_type *sax, 
                           sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           float ratio_sqrt);
float   minidist_paa_to_isax_raw_e_SIMD(float *paa, sax_type *sax, 
                           sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           float ratio_sqrt,
                           float bsf);
                           
enum response paa_from_ts (data_type *ts_in, data_type *paa_out, int segments, int ts_values_per_segment);
enum response sax_from_paa (data_type *paa, sax_type *sax, int segments, 
                            int cardinality, int bit_cardinality);
                            //float* bsearchsimdpaa(float paaseg, float* sax_breakpoints, int candinalityoffset);
#endif
