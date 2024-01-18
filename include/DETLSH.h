#ifndef DETLSH_H
#define DETLSH_H

#include "../config.h"
#include "../globals.h"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <float.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <map>
#include <algorithm>
#include <random>
#include <string>
#include <numeric>
#include <chrono>
#include <unordered_map>

extern "C" {
#include "utils/sax/sax.h"
#include "utils/sax/ts.h"
#include "utils/isax/isax_index.h"
#include "utils/isax/isax_first_buffer_layer.h"
#include "utils/query_support.h"
#include "utils/indexing_support.h"
#include "utils/encoding_support.h"
}

isax_index **idx_lsh;
void INThandler(int);

void breakpoints_selection(isax_index *index, const char *ifilename, long int data_point_num, int maxquerythread, data_type * rawfile);
void* sampling_worker(void *transferdata);
void* breakpoints_selection_worker(void *transferdata);

void load_data(char * dataset, long int dataset_size);

float euclidean_distance(data_type * t, data_type * s, int size);

struct candidate_node{
    float dist;
    long int currentposition;
    bool operator < (const candidate_node node) const {
		return this->dist < node.dist;
	}
};

static size_t getCurrentRSS() {
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    /* OSX ------------------------------------------------------ */
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
        (task_info_t)&info, &infoCount) != KERN_SUCCESS)
        return (size_t)0L;      /* Can't access? */
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    /* Linux ---------------------------------------------------- */
    long rss = 0L;
    FILE *fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t) 0L;      /* Can't open? */
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t) 0L;      /* Can't read? */
    }
    fclose(fp);
    return (size_t) rss * (size_t) sysconf(_SC_PAGESIZE);

#else
    /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
    return (size_t)0L;          /* Unsupported. */
#endif
}

#endif 
