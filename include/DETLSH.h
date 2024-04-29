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
#include <unordered_map>
#include <sys/sysinfo.h>
#include "immintrin.h"
#include <cassert>
#include <omp.h>
#include <mutex>
#include <atomic>

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

void breakpoints_selection(isax_index *index, const char *ifilename, long int data_point_num, data_type * rawfile);
void* sampling_worker(void *transferdata);
void* breakpoints_selection_worker(void *transferdata);

void load_data(char * dataset, long int dataset_size, int data_dimensionality);

float euclidean_distance(data_type * t, data_type * s, int size);
float euclidean_distance_SIMD(float * t, float * s, int size);

struct candidate_node{
    float dist;
    long int currentposition;
    candidate_node(long int position) :currentposition(position){};
    bool operator < (const candidate_node node) const {
		return this->dist < node.dist;
	}
};

bool CompLess(const query_result* qrItem1, const query_result* qrItem2)
{
	return qrItem1->lower_distance < qrItem2->lower_distance;
}

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

namespace faiss {

/*********************************************************
 * Optimized distance/norm/inner prod computations
 *********************************************************/

/// Squared L2 distance between two vectors
float fvec_L2sqr_avx512(
        const float* x,
        const float* y,
        size_t d);

/// inner product
float fvec_inner_product_avx512(
        const float* x,
        const float* y,
        size_t d);

/// L1 distance
float fvec_L1_avx512(
        const float* x,
        const float* y,
        size_t d);

/// infinity distance
float fvec_Linf_avx512(
        const float* x,
        const float* y,
        size_t d);

/// popcnt
int popcnt_AVX512VBMI_lookup(
        const uint8_t* data,
        const size_t n);

/// binary distance
int xor_popcnt_AVX512VBMI_lookup(
        const uint8_t* data1,
        const uint8_t* data2,
        const size_t n);

int or_popcnt_AVX512VBMI_lookup(
        const uint8_t* data1,
        const uint8_t* data2,
        const size_t n);

int and_popcnt_AVX512VBMI_lookup(
        const uint8_t* data1,
        const uint8_t* data2,
        const size_t n);

float jaccard_AVX512(
        const uint8_t* a,
        const uint8_t* b,
        size_t n);

} // namespace faiss

class progress_display
{
public:
    explicit progress_display(
        unsigned long expected_count,
        std::ostream& os = std::cout,
        const std::string& s1 = "\n",
        const std::string& s2 = "",
        const std::string& s3 = "")
        : m_os(os), m_s1(s1), m_s2(s2), m_s3(s3)
    {
        restart(expected_count);
    }
    void restart(unsigned long expected_count)
    {
        //_count = _next_tic_count = _tic = 0;
        _expected_count = expected_count;
        m_os << m_s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
            << m_s2 << "|----|----|----|----|----|----|----|----|----|----|"
            << std::endl
            << m_s3;
        if (!_expected_count)
        {
            _expected_count = 1;
        }
    }
    unsigned long operator += (unsigned long increment)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if ((_count += increment) >= _next_tic_count)
        {
            display_tic();
        }
        return _count;
    }
    unsigned long  operator ++ ()
    {
        return operator += (1);
    }

    //unsigned long  operator + (int x)
    //{
    //	return operator += (x);
    //}

    unsigned long count() const
    {
        return _count;
    }
    unsigned long expected_count() const
    {
        return _expected_count;
    }
private:
    std::ostream& m_os;
    const std::string m_s1;
    const std::string m_s2;
    const std::string m_s3;
    std::mutex mtx;
    std::atomic<size_t> _count{ 0 }, _expected_count{ 0 }, _next_tic_count{ 0 };
    std::atomic<unsigned> _tic{ 0 };
    void display_tic()
    {
        unsigned tics_needed = unsigned((double(_count) / _expected_count) * 50.0);
        do
        {
            m_os << '*' << std::flush;
        } while (++_tic < tics_needed);
        _next_tic_count = unsigned((_tic / 50.0) * _expected_count);
        if (_count == _expected_count)
        {
            if (_tic < 51) m_os << '*';
            m_os << std::endl;
        }
    }
};

#endif 
