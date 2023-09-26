#include "config.h"

#ifndef isax_globals_h
#define isax_globals_h

#define STORE_ANSWER


#pragma GCC diagnostic ignored "-Wunused-result" 
#pragma GCC diagnostic ignored "-Wunused-variable" 
 

#define PAGE_SIZE 4096
#define PROGRESS_CALCULATE_THREAD_NUMBER 12
#define PROGRESS_FLUSH_THREAD_NUMBER 12
#define QUERIES_THREAD_NUMBER 4
#define DISK_BUFFER_SIZE 8192
#define LOCK_SIZE 65536 
///// TYPES /////
typedef unsigned char sax_type;
typedef float data_type;
typedef unsigned long long file_position_type;
typedef unsigned long long root_mask_type;

enum response {OUT_OF_MEMORY_FAILURE, FAILURE, SUCCESS};
enum insertion_mode {PARTIAL = 1, 
                     TMP = 2, 
                     FULL = 4,
                     NO_TMP = 8};

enum buffer_cleaning_mode {FULL_CLEAN, TMP_ONLY_CLEAN, TMP_AND_TS_CLEAN};
enum node_cleaning_mode {DO_NOT_INCLUDE_CHILDREN = 0,
                         INCLUDE_CHILDREN = 1};

///// DEFINITIONS /////
#define MINVAL -2000000
#define MAXVAL 2000000
#define DELIMITER ' '
#define TRUE 1
#define FALSE 0
#define BUFFER_REALLOCATION_RATE  2 

///// GLOBAL VARIABLES /////
int FLUSHES;
unsigned long BYTES_ACCESSED;
float APPROXIMATE;

void* LOGFILE;

#define INCREASE_BYTES_ACCESSED(new_bytes) \
	    BYTES_ACCESSED += (unsigned long) new_bytes;
#define RESET_BYTES_ACCESSED \
		BYTES_ACCESSED = 0;
#define SET_APPROXIMATE(approximate)\
		APPROXIMATE = approximate;

#define SET_LOGFILE(logfile) \
    LOGFILE = logfile;

///// MACROS /////
#define CREATE_MASK(mask, index, sax_array)\
	int mask__i; \
	for (mask__i=0; mask__i < index->settings->LSH_dimensionality; mask__i++) \
		if(index->settings->bit_masks[index->settings->sax_bit_cardinality - 1] & sax_array[mask__i]) \
			mask |= index->settings->bit_masks[index->settings->LSH_dimensionality - mask__i - 1];  

///// BENCHMARKING /////
#ifdef BENCHMARK
		#include <time.h>
		#include <sys/time.h>
	   
        double tS;
        double tE;
        int added_tree_node;
        struct timeval total_time_start;
        struct timeval queue_time_start;
        struct timeval parse_time_start;
        struct timeval input_time_start;
        struct timeval input2_time_start;
        struct timeval indexing_time_start;
        struct timeval querying_time_start;
        struct timeval cal_time_start;
        struct timeval output_time_start;
        struct timeval output2_time_start;
        struct timeval load_node_start;
        struct timeval binning_time_start;
        struct timeval current_time;
        struct timeval fetch_start;
        struct timeval fetch_check_start;
        double total_input_time;
        double total_queue_time;
        double total_input2_time;
        double total_cal_time;
        double load_node_time;
        double total_output_time;
        double total_output2_time;
        double total_parse_time;
        double total_time;
        double total_binning_time;
        double total_indexing_time;
        double total_querying_time;

        double total_querying_time_all;
        unsigned long bytes_accessed_all;
        float approximate_all;
        double result_distance_all;
        double total_time_all;

        struct timeval init_time_start;
        struct timeval tree_pass_time_start;

        double total_init_time;
        double total_tree_pass_time;
        unsigned long int TOTAL_PQ_INSERT_TIME;
        unsigned long int TOTAL_PQ_REMOVE_TIME;
        unsigned long int TOTAL_LB_DIST_CALC_TIME;
        unsigned long int TOTAL_REAL_DIST_CALC_TIME;

        unsigned long int TOTAL_INDEXING_PART_TIME;
        unsigned long int TOTAL_TRANSFORMATION_PART_TIME;

        double total_init_time_all;
        double total_tree_pass_time_all;
        double total_pq_insert_time_all;
        double total_pq_remove_time_all;
        double total_lb_dist_calc_time_all;
        double total_real_dist_calc_time_all;
        int l_size;
        int total_tree_nodes;
        int loaded_nodes;
        int checked_nodes;
        int loaded_nodes_all;
        int checked_nodes_all;
        file_position_type loaded_records;
        unsigned long int LBDcalculationnumber;
        unsigned long int RDcalculationnumber;

        unsigned long int LBDcalculationnumber_all;
        unsigned long int RDcalculationnumber_all;

        #define INIT_STATS() total_input_time = 0;\
                            total_output_time = 0;\
                            total_time = 0;\
                            total_parse_time = 0;\
                            total_tree_nodes = 0;\
                            total_binning_time = 0;\
                            total_indexing_time = 0;\
                            total_querying_time = 0;\
                            loaded_nodes = 0;\
                            checked_nodes = 0;\
                            loaded_nodes_all = 0;\
                            checked_nodes_all = 0;\
                            load_node_time = 0;\
                            loaded_records = 0; \
                            APPROXIMATE=0;\
                            BYTES_ACCESSED=0;\
                            LBDcalculationnumber=0;\
                            RDcalculationnumber=0;\
                            LBDcalculationnumber_all=0;\
                            RDcalculationnumber_all=0;\
                            total_querying_time_all=0.0;\
                            bytes_accessed_all=0;\
                            approximate_all = 0.0;\
                            result_distance_all=0.0;\
                            total_init_time=0.0;\
                            total_tree_pass_time=0.0;\
                            TOTAL_PQ_INSERT_TIME=0;\
                            TOTAL_PQ_REMOVE_TIME=0;\
                            TOTAL_LB_DIST_CALC_TIME=0;\
                            TOTAL_REAL_DIST_CALC_TIME=0;\
                            total_init_time_all=0.0;\
                            total_tree_pass_time_all=0.0;\
                            total_pq_insert_time_all=0.0;\
                            total_pq_remove_time_all=0.0;\
                            total_lb_dist_calc_time_all=0.0;\
                            total_real_dist_calc_time_all=0.0;\
                            TOTAL_INDEXING_PART_TIME = 0.0;\
                            TOTAL_TRANSFORMATION_PART_TIME = 0.0;
                            
        #define PRINT_STATS(result_distance) printf("%lf\t %lf\t %d\t %d\t %ld\t %d\t %lld\t %lf\t %lf\t %lf\n", \
        total_input_time, total_output_time, \
        total_tree_nodes, checked_nodes, \
        BYTES_ACCESSED, loaded_nodes, \
        loaded_records, APPROXIMATE,\
        result_distance, total_time);
        //#define PRINT_STATS(result_distance) printf("%d\t",loaded_nodes);
        #define INIT_INDEX_STATS_FILE(ifile)  fprintf(ifile, "binning,indexing total, transformation, indexing,total time,index file size\n%lf,%lf,%ld,%ld,%lf,", total_binning_time, total_indexing_time,\
        TOTAL_TRANSFORMATION_PART_TIME, TOTAL_INDEXING_PART_TIME, (total_binning_time+total_indexing_time));
        #define INIT_SAVE_FILE(ifile) fprintf(ifile, "querying time, init time, tree pass time, pq insert time, pq remove time, lb dist time, real dist time, nodes, checked_nodes, bytes_accessed, loaded_nodes, loaded_records, real dist calc, lb dist calc, approximate_distance, distance, total\n");
        #define SAVE_STATS(result_distance) fprintf(LOGFILE,"%lf, %lf, %lf, %lu, %lu, %lu, %lu, %d, %d, %ld, %d, %lld, %lu, %lu, %lf, %lf, %lf\n", \
        total_querying_time, total_init_time, \
        total_tree_pass_time, TOTAL_PQ_INSERT_TIME, TOTAL_PQ_REMOVE_TIME,\
        TOTAL_LB_DIST_CALC_TIME, TOTAL_REAL_DIST_CALC_TIME,\
        total_tree_nodes, checked_nodes, \
        BYTES_ACCESSED, loaded_nodes, \
        loaded_records, RDcalculationnumber, \
        LBDcalculationnumber, APPROXIMATE, \
        result_distance, total_time);\
        total_querying_time_all += total_querying_time;\
        total_time_all += total_time;\
        bytes_accessed_all += BYTES_ACCESSED;\
        approximate_all += APPROXIMATE;\
        result_distance_all += result_distance;\
        RDcalculationnumber_all += RDcalculationnumber;\
        RDcalculationnumber = 0;\
        LBDcalculationnumber_all += LBDcalculationnumber;\
        LBDcalculationnumber = 0;\
        checked_nodes_all += checked_nodes;\
        checked_nodes = 0;\
        loaded_nodes_all += loaded_nodes;\
        loaded_nodes = 0;\
        total_init_time_all += total_init_time;\
        total_init_time=0.0;\
        total_tree_pass_time_all += total_tree_pass_time;\
        total_tree_pass_time=0.0;\
        total_pq_insert_time_all += TOTAL_PQ_INSERT_TIME;\
        TOTAL_PQ_INSERT_TIME=0;\
        total_pq_remove_time_all += TOTAL_PQ_REMOVE_TIME;\
        TOTAL_PQ_REMOVE_TIME=0;\
        total_lb_dist_calc_time_all += TOTAL_LB_DIST_CALC_TIME;\
        TOTAL_LB_DIST_CALC_TIME=0;\
        total_real_dist_calc_time_all += TOTAL_REAL_DIST_CALC_TIME;\
        TOTAL_REAL_DIST_CALC_TIME=0;
        #define SAVE_STATS_TOTAL(ifile, queries_size) fprintf(ifile,"%lf, %lf, %lf, %lf, %lf, %lf, %lf, %d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", \
        (total_querying_time_all / queries_size), (total_init_time_all / queries_size),\
        (total_tree_pass_time_all / queries_size), (total_pq_insert_time_all / queries_size),(total_pq_remove_time_all / queries_size),\
        (total_lb_dist_calc_time_all / queries_size), (total_real_dist_calc_time_all / queries_size),\
        total_tree_nodes, ((double)checked_nodes_all / queries_size), \
        ((double)bytes_accessed_all / queries_size), ((double)loaded_nodes_all / queries_size), \
        ((double)loaded_records / queries_size), ((double)RDcalculationnumber_all / queries_size), \
        ((double)LBDcalculationnumber_all / queries_size), (approximate_all / queries_size),\
        (result_distance_all / queries_size), (total_time_all / queries_size));
        #define min(x,y)  ( x<y?x:y )
        #define COUNT_NEW_NODE() __sync_fetch_and_add(&total_tree_nodes,1);
        #define COUNT_LOADED_NODE() loaded_nodes++;
        #define COUNT_CHECKED_NODE() checked_nodes++;
        #define COUNT_LOADED_RECORD() loaded_records++;

        #define COUNT_INIT_TIME_START gettimeofday(&init_time_start, NULL);
        #define COUNT_TREE_PASS_TIME_START gettimeofday(&tree_pass_time_start, NULL);
        #define COUNT_PQ_INSERT_TIME_START gettimeofday(&pq_insert_time_start, NULL);

        #define COUNT_INIT_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = init_time_start.tv_sec*1000000 + (init_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_init_time += (tE - tS);
        #define COUNT_TREE_PASS_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = tree_pass_time_start.tv_sec*1000000 + (tree_pass_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_tree_pass_time += (tE - tS);                        

        #define COUNT_INPUT_TIME_START gettimeofday(&input_time_start, NULL);
        #define COUNT_QUEUE_TIME_START gettimeofday(&queue_time_start, NULL);
        #define COUNT_INDEXING_TIME_START gettimeofday(&indexing_time_start, NULL);
        #define COUNT_QUERYING_TIME_START gettimeofday(&querying_time_start, NULL); \
          total_querying_time = 0.0;
        #define COUNT_CAL_TIME_START gettimeofday(&cal_time_start, NULL); 
        #define COUNT_INPUT2_TIME_START gettimeofday(&input2_time_start, NULL); 
        #define COUNT_OUTPUT_TIME_START gettimeofday(&output_time_start, NULL); 
        #define COUNT_OUTPUT2_TIME_START gettimeofday(&output2_time_start, NULL);
        #define COUNT_TOTAL_TIME_START gettimeofday(&total_time_start, NULL);   
        #define COUNT_PARSE_TIME_START gettimeofday(&parse_time_start, NULL);   
        #define COUNT_LOAD_NODE_START gettimeofday(&load_node_start, NULL);
        #define COUNT_BINNING_TIME_START gettimeofday(&binning_time_start, NULL);
        #define COUNT_BINNING_TIME_END gettimeofday(&current_time, NULL); \
                                      tS = binning_time_start.tv_sec*1000000 + (binning_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_binning_time += (tE - tS);
        #define COUNT_INDEXING_TIME_END gettimeofday(&current_time, NULL); \
                                      tS = indexing_time_start.tv_sec*1000000 + (indexing_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_indexing_time += (tE - tS);
        #define COUNT_QUERYING_TIME_END gettimeofday(&current_time, NULL); \
                                      tS = querying_time_start.tv_sec*1000000 + (querying_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_querying_time += (tE - tS);
        #define COUNT_INPUT_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = input_time_start.tv_sec*1000000 + (input_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_input_time += (tE - tS); 
        #define COUNT_INPUT2_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = input2_time_start.tv_sec*1000000 + (input2_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_input2_time += (tE - tS); 
        #define COUNT_CAL_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = cal_time_start.tv_sec*1000000 + (cal_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
                                      total_cal_time += (tE - tS); 
        #define COUNT_OUTPUT_TIME_END gettimeofday(&current_time, NULL); \
                                      tS = output_time_start.tv_sec*1000000 + (output_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      total_output_time += (tE - tS); 
        #define COUNT_OUTPUT2_TIME_END gettimeofday(&current_time, NULL); \
                                      tS = output2_time_start.tv_sec*1000000 + (output2_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      total_output2_time += (tE - tS); 
        #define COUNT_TOTAL_TIME_END  gettimeofday(&current_time, NULL); \
                                      tS = total_time_start.tv_sec*1000000 + (total_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      total_time += (tE - tS); 
        #define COUNT_PARSE_TIME_END  gettimeofday(&current_time, NULL);  \
                                      tS = parse_time_start.tv_sec*1000000 + (parse_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      total_parse_time += (tE - tS); 
        #define COUNT_LOAD_NODE_END   gettimeofday(&current_time, NULL);  \
                                      tS = load_node_start.tv_sec*1000000 + (load_node_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      load_node_time += (tE - tS); 
        #define COUNT_QUEUE_TIME_END  gettimeofday(&current_time, NULL);  \
                                      tS = queue_time_start.tv_sec*1000000 + (queue_time_start.tv_usec); \
                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
                                      total_queue_time += (tE - tS); 
    #else
        #define INIT_STATS() ;
        #define PRINT_STATS() ;
        #define COUNT_NEW_NODE() ;
        #define COUNT_CHECKED_NODE();
        #define COUNT_LOADED_NODE() ;
        #define COUNT_LOADED_RECORD() ;
        #define COUNT_INPUT_TIME_START ;
        #define COUNT_INPUT_TIME_END ;
        #define COUNT_OUTPUT_TIME_START ;
        #define COUNT_OUTPUT_TIME_END ;
        #define COUNT_TOTAL_TIME_START ;
        #define COUNT_TOTAL_TIME_END ;
        #define COUNT_PARSE_TIME_START ;
        #define COUNT_PARSE_TIME_END ;
    #endif
#endif
