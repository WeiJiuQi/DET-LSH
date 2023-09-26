#ifndef isaxlib_ts_h
#define isaxlib_ts_h
#include "../../../config.h"
#include "../../../globals.h"
void ts_parse_str(char ts_str[], data_type *ts_out, int ts_size, const char * delims);
void ts_print(data_type *ts, int size);
float ts_euclidean_distance(data_type * t, data_type * s, int size, float bound);
float ts_euclidean_distance_SIMD(data_type * t, data_type * s, int size, float bound);
float ts_euclidean_distance_neSIMD(data_type * t, data_type * s, int size);
#endif
