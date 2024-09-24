#include "DETLSH.h"

#ifdef VALUES
#include <values.h>
#endif

#define FILENAME_LENGTH 256


int main (int argc, char **argv)
{
	signal(SIGINT, INThandler);

    static char index_directory[FILENAME_LENGTH];
    strcpy(index_directory, getenv("HOME"));
    strcat(index_directory, "/index/");

    static char data_directory[FILENAME_LENGTH];
    strcpy(data_directory, getenv("HOME"));
    strcat(data_directory, "/data/data");

    static char query_directory[FILENAME_LENGTH];
    strcpy(query_directory, getenv("HOME"));
    strcat(query_directory, "/data/query");

    static char groundtruth_directory[FILENAME_LENGTH];
    strcpy(groundtruth_directory, getenv("HOME"));
    strcat(groundtruth_directory, "/data/groundtruth");

    static char * dataset = data_directory;
    static char * queries = query_directory;
    static char * index_path = index_directory;
    static char * groundtruth = groundtruth_directory;
    static long int dataset_size = 1000000;
    static int queries_size = 100;
    static int data_dimensionality = 128;
    static int LSH_dimensionality = 16;
    static int sax_cardinality = 8;
    static int leaf_size = 2000;
    static int min_leaf_size = 10;
    static int initial_lbl_size = 2000;
    static int flush_limit = 300000;
    static int initial_fbl_size = 100;
    static int total_loaded_leaves = 1;
    static int cpu_control_type = 1;
    static char SIMD_flag=0;
    static int sample_type=2;

    l_size = 4;
    N_PQUEUE = 1;
    num_thread = 1;
    int k_size = 50;
    float search_radius = 1;
    int max_candidate_size = 1;
    int sample_size = 1;

    while (1)
    {
        static struct option long_options[] =  {
            {"initial-lbl-size", required_argument, 0, 'b'},
            {"dataset", required_argument, 0, 'd'},
            {"total-loaded-leaves", required_argument, 0, 'e'},
            {"flush-limit", required_argument, 0, 'f'},
            {"help", no_argument, 0, 'h'},
            {"initial-fbl-size", required_argument, 0, 'i'},
            {"queries-size", required_argument, 0, 'k'},
            {"leaf-size", required_argument, 0, 'l'},
            {"min-leaf-size", required_argument, 0, 'm'},
            {"index-path", required_argument, 0, 'p'},
            {"queries", required_argument, 0, 'q'},
            {"data-dimensionality", required_argument, 0, 't'},
            {"cpu-type", required_argument, 0, 'w'},
            {"sax-cardinality", required_argument, 0, 'x'},
            {"LSH-dimensionality", required_argument, 0, 'B'},
            {"dataset-size", required_argument, 0, 'z'},
            {"k-size", required_argument, 0, '0'},
            {"queue-number", required_argument, 0, '6'},
            {"SIMD", no_argument, 0, '7'},
            {"sample-size", required_argument, 0, '8'},
            {"sample-type", required_argument, 0, 'C'},
            {"l-size", required_argument, 0, 'E'},
            {"search-radius", required_argument, 0, 'F'},
            {"max-candidate-size", required_argument, 0, 'G'},
            {"groundtruth", required_argument, 0, 'H'},
            {NULL, 0, NULL, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;
        int c = getopt_long (argc, argv, "",
                             long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
            case 'e':
                total_loaded_leaves = atoi(optarg);
                break;

            case 'q':
                queries = optarg;
                break;

            case 'k':
                queries_size = atoi(optarg);
                break;

            case 'd':
                dataset = optarg;
                break;

            case 'p':
                index_path = optarg;
                break;

            case 'z':
                dataset_size = atoi(optarg);
                break;

            case 't':
                data_dimensionality = atoi(optarg);
                break;

            case 'x':
                sax_cardinality = atoi(optarg);
                break;

            case 'B':
            	LSH_dimensionality = atoi(optarg);
		break;
		
            case 'l':
                leaf_size = atoi(optarg);
                break;

            case 'm':
                min_leaf_size = atoi(optarg);
                break;

            case 'b':
                initial_lbl_size = atoi(optarg);
                break;

            case 'f':
                flush_limit = atoi(optarg);
                break;

            case 'w':
                cpu_control_type = atoi(optarg);
                break;
            case 'i':
                initial_fbl_size = atoi(optarg);
                break;
            case '0':
                k_size = atoi(optarg);
                break;
            case '6':
                N_PQUEUE =  atoi(optarg);
                break;
            case '8':
                sample_size = atoi(optarg);
                break;
            case 'C':
                sample_type = atoi(optarg);
                break;
            case 'E':
                l_size = atoi(optarg);
                break;
            case 'F':
                search_radius = atof(optarg);
                break;
            case 'G':
                max_candidate_size = atoi(optarg);
                break;
            case 'H':
                groundtruth = optarg;
                break;

                printf("Usage:\n\
                \t--dataset XX \t\t\tThe path to the dataset file\n\
                \t--queries XX \t\t\tThe path to the queries file\n\
                \t--dataset-size XX \t\tThe number of time series to load\n\
                \t--queries-size XX \t\tThe number of queries to do\n\
                \t--index-path XX \t\tThe path of the output folder\n\
                \t--data-dimensionality XX\t\tThe size of each data point\n\
                \t--sax-cardinality XX\t\tThe maximum sax cardinality in number of bits (power of two).\n\
                \t--LSH-dimensionality XX\t\tThe dimensionality of each projected space.\n\
                \t--leaf-size XX\t\t\tThe maximum size of each leaf\n\
                \t--min-leaf-size XX\t\tThe minimum size of each leaf\n\
                \t--initial-lbl-size XX\t\tThe initial lbl buffer size for each buffer.\n\
                \t--flush-limit XX\t\tThe limit of time series in memory at the same time\n\
                \t--initial-fbl-size XX\t\tThe initial fbl buffer size for each buffer.\n\
                \t--total-loaded-leaves XX\tNumber of leaves to load at each fetch\n\
                \t--queue-number\t\tset the number of priority queues\n\
                \t--SIMD\t\t\tSet for search with SIMD intrinsics\n\
                \t--sample-size\t\t\tSet sample size for MCB\n\
                \t--sample-type\t\t\tSet for sampling strategy\n\
                \t\t\tfirst-n-values sampling: 1\n\
                \t\t\tuniform sampling: 2\n\
                \t\t\trandom sampling: 3\n\
                \t--cpu-type\t\t\tSet for how many cores you want to used and in 1 or 2 cpu\n\
                \t--help\n\n\
                \tCPU type code:\t\t\t21 : 2 core in 1 CPU\n\
                \t\t\t\t\t22 : 2 core in 2 CPUs\n\
                \t\t\t\t\t41 : 4 core in 1 CPU\n\
                \t\t\t\t\t42 : 4 core in 2 CPUs\n\
                \t\t\t\t\t61 : 6 core in 1 CPU\n\
                \t\t\t\t\t62 : 6 core in 2 CPUs\n\
                \t\t\t\t\t81 : 8 core in 1 CPU\n\
                \t\t\t\t\t82 : 8 core in 2 CPUs\n\
                \t\t\t\t\t101: 10 core in 1 CPU\n\
                \t\t\t\t\t102: 10 core in 2 CPUs\n\
                \t\t\t\t\t121: 12 core in 1 CPU\n\
                \t\t\t\t\t122: 12 core in 2 CPUs\n\
                \t\t\t\t\t181: 18 core in 1 CPU\n\
                \t\t\t\t\t182: 18 core in 2 CPUs\n\
                \t\t\t\t\t242: 24 core in 2 CPUs\n\
                \t\t\t\t\tOther: 1 core in 1 CPU\n\
                ");
                return 0;
                break;
            case '7':
                SIMD_flag = 1;
                break;
            default:
                exit(-1);
                break;
        }
    }
    initial_lbl_size = leaf_size;
    min_leaf_size = leaf_size;
    INIT_STATS();
	cpu_set_t mask,get;
    CPU_ZERO(&mask);
    CPU_ZERO(&get);

    if (cpu_control_type < 0) {
        fprintf(stderr, "threads number must greater than 0\n");
        exit(-1);
    }
    else if(cpu_control_type==21)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        num_thread=2;
    }
    else if(cpu_control_type==22)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        num_thread=2;
    }
    else if(cpu_control_type==41)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        num_thread=4;
    }
    else if(cpu_control_type==42)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        num_thread=4;
    }
    else if(cpu_control_type==61)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        num_thread=6;
    }
    else if(cpu_control_type==62)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        num_thread=6;
    }
    else if (cpu_control_type==81)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        CPU_SET(12, &mask);
        CPU_SET(14, &mask);
        num_thread=8;
    }
    else if (cpu_control_type==82)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        CPU_SET(6, &mask);
        CPU_SET(7, &mask);
        num_thread=8;
    }
    else if (cpu_control_type==101)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        CPU_SET(12, &mask);
        CPU_SET(14, &mask);
        CPU_SET(16, &mask);
        CPU_SET(18, &mask);
        num_thread=10;
    }
    else if (cpu_control_type==102)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        CPU_SET(6, &mask);
        CPU_SET(7, &mask);
        CPU_SET(8, &mask);
        CPU_SET(9, &mask);
        num_thread=10;
    }
    else if (cpu_control_type==121)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        CPU_SET(12, &mask);
        CPU_SET(14, &mask);
        CPU_SET(16, &mask);
        CPU_SET(18, &mask);
        CPU_SET(20, &mask);
        CPU_SET(22, &mask);
        num_thread=12;
    }
    else if (cpu_control_type==122)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        CPU_SET(6, &mask);
        CPU_SET(7, &mask);
        CPU_SET(8, &mask);
        CPU_SET(9, &mask);
        CPU_SET(10, &mask);
        CPU_SET(11, &mask);
        num_thread=12;
    }
    else if (cpu_control_type==141)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        CPU_SET(12, &mask);
        CPU_SET(14, &mask);
        CPU_SET(16, &mask);
        CPU_SET(18, &mask);
        CPU_SET(20, &mask);
        CPU_SET(22, &mask);
        CPU_SET(24, &mask);
        CPU_SET(26, &mask);
        num_thread=14;
    }
    else if (cpu_control_type==142)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        CPU_SET(6, &mask);
        CPU_SET(7, &mask);
        CPU_SET(8, &mask);
        CPU_SET(9, &mask);
        CPU_SET(10, &mask);
        CPU_SET(11, &mask);
        CPU_SET(12, &mask);
        CPU_SET(13, &mask);
        num_thread=14;
    }
    else if (cpu_control_type==161)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        CPU_SET(12, &mask);
        CPU_SET(14, &mask);
        CPU_SET(16, &mask);
        CPU_SET(18, &mask);
        CPU_SET(20, &mask);
        CPU_SET(22, &mask);
        CPU_SET(24, &mask);
        CPU_SET(26, &mask);
        CPU_SET(28, &mask);
        CPU_SET(30, &mask);
        num_thread=16;
    }
    else if (cpu_control_type==162)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        CPU_SET(6, &mask);
        CPU_SET(7, &mask);
        CPU_SET(8, &mask);
        CPU_SET(9, &mask);
        CPU_SET(10, &mask);
        CPU_SET(11, &mask);
        CPU_SET(12, &mask);
        CPU_SET(13, &mask);
        CPU_SET(14, &mask);
        CPU_SET(15, &mask);
        num_thread=16;
    }
    else if (cpu_control_type==182)
    {
            CPU_SET(0, &mask);
            CPU_SET(1, &mask);
            CPU_SET(2, &mask);
            CPU_SET(3, &mask);
            CPU_SET(4, &mask);
            CPU_SET(5, &mask);
            CPU_SET(6, &mask);
            CPU_SET(7, &mask);
            CPU_SET(8, &mask);
            CPU_SET(9, &mask);
            CPU_SET(10, &mask);
            CPU_SET(11, &mask);
            CPU_SET(12, &mask);
            CPU_SET(13, &mask);
            CPU_SET(14, &mask);
            CPU_SET(15, &mask);
            CPU_SET(16, &mask);
            CPU_SET(17, &mask);
            num_thread=18;
    }
    else if (cpu_control_type==242)
    {
            CPU_SET(0, &mask);
            CPU_SET(1, &mask);
            CPU_SET(2, &mask);
            CPU_SET(3, &mask);
            CPU_SET(4, &mask);
            CPU_SET(5, &mask);
            CPU_SET(6, &mask);
            CPU_SET(7, &mask);
            CPU_SET(8, &mask);
            CPU_SET(9, &mask);
            CPU_SET(10, &mask);
            CPU_SET(11, &mask);
            CPU_SET(12, &mask);
            CPU_SET(13, &mask);
            CPU_SET(14, &mask);
            CPU_SET(15, &mask);
            CPU_SET(16, &mask);
            CPU_SET(17, &mask);
            CPU_SET(18, &mask);
            CPU_SET(19, &mask);
            CPU_SET(20, &mask);
            CPU_SET(21, &mask);
            CPU_SET(22, &mask);
            CPU_SET(23, &mask);
            num_thread=24;
    }
    else if (cpu_control_type==362)
    {
            CPU_SET(0, &mask);
            CPU_SET(1, &mask);
            CPU_SET(2, &mask);
            CPU_SET(3, &mask);
            CPU_SET(4, &mask);
            CPU_SET(5, &mask);
            CPU_SET(6, &mask);
            CPU_SET(7, &mask);
            CPU_SET(8, &mask);
            CPU_SET(9, &mask);
            CPU_SET(10, &mask);
            CPU_SET(11, &mask);
            CPU_SET(12, &mask);
            CPU_SET(13, &mask);
            CPU_SET(14, &mask);
            CPU_SET(15, &mask);
            CPU_SET(16, &mask);
            CPU_SET(17, &mask);
            CPU_SET(18, &mask);
            CPU_SET(19, &mask);
            CPU_SET(20, &mask);
            CPU_SET(21, &mask);
            CPU_SET(22, &mask);
            CPU_SET(23, &mask);
            CPU_SET(24, &mask);
            CPU_SET(25, &mask);
            CPU_SET(26, &mask);
            CPU_SET(27, &mask);
            CPU_SET(28, &mask);
            CPU_SET(29, &mask);
            CPU_SET(30, &mask);
            CPU_SET(31, &mask);
            CPU_SET(32, &mask);
            CPU_SET(33, &mask);
            CPU_SET(34, &mask);
            CPU_SET(35, &mask);
            num_thread=36;
    }
    else
    {
        num_thread=cpu_control_type;
    }


    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) 
    {
        fprintf(stderr, "set thread affinity failed\n");
    }

    if (pthread_getaffinity_np(pthread_self(), sizeof(get), &get) < 0) 
    {
        fprintf(stderr, "get thread affinity failed\n");
    }

    // Initialize indexes
    isax_index_settings * index_settings = isax_index_settings_init(index_path,             // INDEX DIRECTORY
                                                                        data_dimensionality,   // DATA DIMENSIONALITY
                                                                        LSH_dimensionality,       // LSH DIMENSIONALITY
                                                                        sax_cardinality,    // SAX CARDINALITY IN BITS
                                                                        leaf_size,          // LEAF SIZE
                                                                        min_leaf_size,      // MIN LEAF SIZE
                                                                        initial_lbl_size,   // INITIAL LEAF BUFFER SIZE
                                                                        flush_limit,        // FLUSH LIMIT
                                                                        initial_fbl_size,   // INITIAL FBL BUFFER SIZE
                                                                        total_loaded_leaves,// Leaves to load at each fetch
                                                                        1,                  // new index
                                                                        SIMD_flag,          //SIMD_flag
                                                                        sample_size,        //sample_size for MCB
                                                                        sample_type);		//sampling type
    idx_lsh = (isax_index **) calloc(l_size,sizeof(isax_index*));
    for (int i = 0; i < l_size; i++)
    {
        idx_lsh[i] = isax_index_init_inmemory(index_settings);
        idx_lsh[i]->range_queue_result = (pqueue_result *) calloc(queries_size, sizeof(pqueue_result));
    }

    std::mt19937 rng(unsigned(0));
    // std::mt19937 rng(std::random_device{}());
    std::normal_distribution<float> nd;

    struct timeval start, end;
    long query_all, index_all;

    // Load data
    dataset_size = dataset_size - 100;
    load_data(dataset, dataset_size, data_dimensionality);
    
    int memory_before_indexing = getCurrentRSS() / 1000000;
    std::cout << "Actual memory usage before indexing: " << getCurrentRSS() / 1000000 << " MB" << std::endl;

    // Generate LSH representations
    for (int num_l = 0; num_l < l_size; num_l++)
    {
        std::cout << "Generating LSH vectors and calculating the LSH representation for all data in the " << num_l << "th index tree." << std::endl;
        // Initialize LSH vectors
        idx_lsh[num_l]->lsh_hash_set = (data_type **) calloc(LSH_dimensionality, sizeof(data_type*));
        for (int i = 0; i < LSH_dimensionality; i++)
        {
            idx_lsh[num_l]->lsh_hash_set[i] = (data_type *) calloc(data_dimensionality, sizeof(data_type));
            
            for(int j = 0; j < data_dimensionality; j++)
            {
                idx_lsh[num_l]->lsh_hash_set[i][j] = (data_type) (nd(rng));
            }
        }

        // Generate LSH representations for all points
        idx_lsh[num_l]->lsh_mem_array = (data_type **) calloc(dataset_size, sizeof(data_type*));
        idx_lsh[num_l]->lsh_mem_array_for_init = (data_type **) calloc(LSH_dimensionality, sizeof(data_type*));

        for (int i = 0; i < LSH_dimensionality; i++)
        {
            idx_lsh[num_l]->lsh_mem_array_for_init[i] = (data_type *) calloc(dataset_size, sizeof(data_type));
        }

        for (long i = 0; i < dataset_size; i++)
        {
            idx_lsh[num_l]->lsh_mem_array[i] = (data_type *) calloc(LSH_dimensionality, sizeof(data_type));
        }

        for (long i = 0; i < dataset_size; i++)
        {
            for(int j = 0; j < LSH_dimensionality; j++)
            {
                data_type value = 0.0;
                for(int k = 0; k < data_dimensionality; k++)
                {
                    value += idx_lsh[num_l]->lsh_hash_set[j][k] * rawfile[i * data_dimensionality + k];
                }
                idx_lsh[num_l]->lsh_mem_array[i][j] = value;
                idx_lsh[num_l]->lsh_mem_array_for_init[j][i] = value;
            }
        }
    }

    // Encoding and indexing
    gettimeofday(&start, NULL);
    for (int num_l = 0; num_l < l_size; num_l++)
    {
        // Breakpoints selection
        breakpoints_init(idx_lsh[num_l]);
        breakpoints_selection(idx_lsh[num_l], dataset, dataset_size, rawfile);

        // Encoding and constructing indexes
        index_creation(dataset_size, idx_lsh[num_l]);
    }
    gettimeofday(&end, NULL);
    index_all += (1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec);

    // Free memory
    for (int num_l = 0; num_l < l_size; num_l++)
    {
        // Free memory for breakpoints selection
        for (int i = 0; i < LSH_dimensionality; i++)
        {
            free(idx_lsh[num_l]->lsh_mem_array_for_init[i]);
        }
        free(idx_lsh[num_l]->lsh_mem_array_for_init);

        // Free memory for encoding and indexing
        for (int i = 0; i < dataset_size; i++)
        {
            free(idx_lsh[num_l]->lsh_mem_array[i]);
        }
        free(idx_lsh[num_l]->lsh_mem_array); 
    }

    int memory_after_indexing = getCurrentRSS() / 1000000;
    std::cout << "Actual memory usage after indexing: " << getCurrentRSS() / 1000000 << " MB" << std::endl;

    // Load and generate LSH representations for queries
    std::cout << "Load and generate LSH representations for queries." << std::endl;
    FILE *ifile_query;
    ifile_query = fopen (queries,"rb");

    if (ifile_query == NULL) {
        fprintf(stderr, "File %s not found!\n", queries);
        exit(-1);
    }

    fseek(ifile_query, 0L, SEEK_END);
    file_position_type sz = (file_position_type) ftell(ifile_query);
    file_position_type total_records = sz/idx_lsh[0]->settings->point_byte_size;
    fseek(ifile_query, 0L, SEEK_SET);

    if (total_records < queries_size) {
        fprintf(stderr, "File %s has only %llu records!\n", queries, total_records);
        exit(-1);
    }

    data_type* queryfile;
    queryfile = (data_type*)malloc(sizeof(data_type) * data_dimensionality * queries_size);
    int query_read_number = fread(queryfile, sizeof(data_type), data_dimensionality * queries_size, ifile_query);
    fclose(ifile_query);

    for (int i = 0; i < l_size; i++)
    {
        idx_lsh[i]->lsh_mem_array_for_query = (data_type **) calloc(queries_size, sizeof(data_type*));
        idx_lsh[i]->query_points = (data_type **) calloc(queries_size, sizeof(data_type*));
        for (int j = 0; j < queries_size; j++)
        {
            idx_lsh[i]->lsh_mem_array_for_query[j] = (data_type *) calloc(LSH_dimensionality, sizeof(data_type));
            idx_lsh[i]->query_points[j] = (data_type *) calloc(data_dimensionality, sizeof(data_type));
        }
    }

    for (int q_loaded = 0; q_loaded < queries_size; q_loaded++)
    {
        data_type * querypoint = (data_type *)malloc(sizeof(data_type) * data_dimensionality);
        memcpy(querypoint, &(queryfile[q_loaded * data_dimensionality]), sizeof(data_type)* data_dimensionality);
        for (int i = 0; i < l_size; i++)
        {                                      
            for(int j = 0; j < LSH_dimensionality; j++)
            {
                data_type value = 0.0;
                for(int k = 0; k < data_dimensionality; k++)
                {
                    idx_lsh[i]->query_points[q_loaded][k] = querypoint[k];
                    value += idx_lsh[i]->lsh_hash_set[j][k] * querypoint[k];
                }
                idx_lsh[i]->lsh_mem_array_for_query[q_loaded][j] = value;
            }  
        }                             
    } 

    struct timeval start_phase_q, end_phase_q;
    long query_phase1, query_phase2, query_phase3;
    
    std::cout << "-----------------Start query-----------------" << std::endl;
    // The first step of query: generate the priority queue of candidate leaf nodes for each query
    gettimeofday(&start, NULL);
    gettimeofday(&start_phase_q, NULL);
    for (int num_l = 0; num_l < l_size; num_l++)
    {     
        range_query(queries, queries_size, idx_lsh[num_l], search_radius, &range_search_lsh);
    }
    gettimeofday(&end_phase_q, NULL);
    gettimeofday(&end, NULL);
    query_all += (1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec);
    query_phase1 += (1000000 * (end_phase_q.tv_sec - start_phase_q.tv_sec) + end_phase_q.tv_usec - start_phase_q.tv_usec);

    // Perform subsequent steps for each query
    candidate_node ** nodes = (candidate_node **)malloc(sizeof(candidate_node*) * queries_size);

    progress_display pd_query(queries_size);
    for (int q_loaded = 0; q_loaded < queries_size; ++q_loaded)
    {
        // The second step of query: generate the candidate set of points by obtained priority queues
        gettimeofday(&start, NULL);

        gettimeofday(&start_phase_q, NULL);

        int data_loaded = 0;

        nodes[q_loaded] = (candidate_node *)malloc(sizeof(candidate_node) * max_candidate_size);

        if (num_thread == 1) {
            std::unordered_map<long int, bool> isCandidate;

            index_query_result * index_query_results = (index_query_result *)malloc(sizeof(index_query_result) * l_size);
            for (int i = 0; i < l_size; i++) { 
                index_query_results[i].result = (query_result **)malloc(sizeof(query_result *) * N_PQUEUE);
            }

            int ** queue_is_empty = (int **) malloc(l_size * sizeof(int*));
            for (int i = 0; i < l_size; i++) {
                queue_is_empty[i] = (int *) malloc(sizeof(int) * N_PQUEUE);
                for (int j = 0; j < N_PQUEUE; j++) {
                    queue_is_empty[i][j] = 0;
                }
            }

            float * mindist_in_every_index = (float *)malloc(sizeof(float) * l_size);
            int * num_queue = (int *)malloc(sizeof(int) * l_size);
            for (int i = 0; i < l_size; i++) {
                mindist_in_every_index[i] = FLT_MAX;
                num_queue[i] = 0;
            }

            for (int i = 0; i < l_size; i++)
            {
                for (int j = 0; j < N_PQUEUE; j++) {
                    index_query_results[i].result[j] = (query_result *)pqueue_pop(idx_lsh[i]->range_queue_result[q_loaded].pq[j]);     

                    if(!index_query_results[i].result[j]) { 
                        queue_is_empty[i][j] = 1;        
                        continue;
                    }
                    
                    if (index_query_results[i].result[j]->lower_distance < mindist_in_every_index[i]) {
                        mindist_in_every_index[i] = index_query_results[i].result[j]->lower_distance;
                        num_queue[i] = j;
                    }
                }
            }

            while(1) 
            {
                int mindist_index = 0;
                float mindist = FLT_MAX;

                for (int i = 0; i < l_size; i++)
                {
                    int accumulated = 0;
                    for (int j = 0; j < N_PQUEUE; j++) {
                        accumulated += queue_is_empty[i][j];
                    }
                    if (accumulated < N_PQUEUE && mindist_in_every_index[i] < mindist) {
                        mindist = mindist_in_every_index[i];
                        mindist_index = i;
                    }
                }

                if (mindist == FLT_MAX) {
                    break;
                }

                for (int i = 0; i < index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_buffer_size; i++)
                {
                    if (isCandidate.find(*index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_position_buffer[i])==isCandidate.end()) {
                        nodes[q_loaded][data_loaded].currentposition = *index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_position_buffer[i] / data_dimensionality;                      
                        isCandidate[*index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_position_buffer[i]] = true;
                        data_loaded++;

                        if (data_loaded == max_candidate_size) {
                            break;
                        }
                    }                            
                }

                if (data_loaded == max_candidate_size) {
                    break;
                }
                
                index_query_results[mindist_index].result[num_queue[mindist_index]] = (query_result *)pqueue_pop(idx_lsh[mindist_index]->range_queue_result[q_loaded].pq[num_queue[mindist_index]]);
                if(!index_query_results[mindist_index].result[num_queue[mindist_index]]) {
                    queue_is_empty[mindist_index][num_queue[mindist_index]] = 1;
                }

                int temp_index = 0;
                float temp_mindist = FLT_MAX;
                for (int j = 0; j < N_PQUEUE; j++) {
                    if (!queue_is_empty[mindist_index][j] && index_query_results[mindist_index].result[j]->lower_distance < temp_mindist) {
                        temp_index = j;
                        temp_mindist = index_query_results[mindist_index].result[j]->lower_distance;
                    }
                }
                mindist_in_every_index[mindist_index] = temp_mindist;
                num_queue[mindist_index] = temp_index;
            }
        } else {
            std::vector<query_result*> leaf_nodes;
            for (int i = 0; i < l_size; i++) {
                for (int j = 0; j < N_PQUEUE; j++) {
                    while (pqueue_size(idx_lsh[i]->range_queue_result[q_loaded].pq[j]) > 0) {
                        leaf_nodes.push_back((query_result *) pqueue_pop(idx_lsh[i]->range_queue_result[q_loaded].pq[j]));
                    }
                }
            }
            sort(leaf_nodes.begin(), leaf_nodes.end(), CompLess);

            std::vector<bool> isCandidate(dataset_size);
            int max_candidate_each_thread = max_candidate_size / num_thread;
            int * num_candidate_each_thread = new int[num_thread]();
            int finished_thread = 0;
            int last_node_index = 0;
            int * last_node_index_each_thread = new int[num_thread]();
            bool stop_to_traverse = false;

            omp_lock_t lock;
            omp_init_lock(&lock);
            
            #pragma omp parallel for schedule(dynamic) num_threads(num_thread)
            for (int i = 0; i < leaf_nodes.size(); i++) {
                if (stop_to_traverse) {
                    continue;
                }

                int tid = omp_get_thread_num();

                last_node_index_each_thread[tid] = i;

                for (int j = 0; j < leaf_nodes[i]->node->buffer->partial_buffer_size; j++) {
                    isCandidate[*leaf_nodes[i]->node->buffer->partial_position_buffer[j] / data_dimensionality] = true;
                }

                num_candidate_each_thread[tid] += leaf_nodes[i]->node->buffer->partial_buffer_size;

                if (num_candidate_each_thread[tid] > max_candidate_each_thread) {
                    omp_set_lock(&lock);
                    finished_thread++;
                    omp_unset_lock(&lock);
                    
                    while(1) {
                        if (finished_thread == num_thread) {
                            stop_to_traverse = true;
                            break;
                        }
                    }
                }
            }
            last_node_index = *std::max_element(last_node_index_each_thread, last_node_index_each_thread + num_thread);

            int obtained_candidate_num = 0;
            #pragma omp parallel for reduction(+:obtained_candidate_num) num_threads(num_thread)
            for (int i = 0; i < dataset_size; i++) {
                if (isCandidate[i] == true) {
                    obtained_candidate_num++;
                }
            }
            int left_candidate_num = max_candidate_size - obtained_candidate_num;

            if (left_candidate_num > 0 && last_node_index != leaf_nodes.size() - 1) 
            {
                for (int i = last_node_index + 1; i < leaf_nodes.size(); i++) {
                    for (int j = 0; j < leaf_nodes[i]->node->buffer->partial_buffer_size; j++) {
                        if (isCandidate[*leaf_nodes[i]->node->buffer->partial_position_buffer[j] / data_dimensionality] == false && left_candidate_num > 0) {
                            isCandidate[*leaf_nodes[i]->node->buffer->partial_position_buffer[j] / data_dimensionality] = true;
                            left_candidate_num--;
                        }
                    }
                    if (left_candidate_num == 0) {
                        break;
                    }
                }
            }

            std::vector<std::vector<candidate_node>> candidate_each_thread(num_thread);
            #pragma omp parallel for num_threads(num_thread)
            for (int i = 0; i < dataset_size; i++) {
                if (isCandidate[i] == true) {
                    candidate_each_thread[omp_get_thread_num()].push_back(candidate_node(i));
                }
            }

            for (int i = 0; i < num_thread; i++) {
                std::copy(candidate_each_thread[i].begin(), candidate_each_thread[i].begin() + candidate_each_thread[i].size(), nodes[q_loaded] + data_loaded);
                data_loaded += candidate_each_thread[i].size();
            }
        }
        gettimeofday(&end_phase_q, NULL);
        query_phase2 += (1000000 * (end_phase_q.tv_sec - start_phase_q.tv_sec) + end_phase_q.tv_usec - start_phase_q.tv_usec);

        // The third step of query: obtain the k-NN from the points in the candidate set
        gettimeofday(&start_phase_q, NULL);
        if (num_thread == 1) {
            for (int i = 0; i < data_loaded; i++) {
                nodes[q_loaded][i].dist = euclidean_distance(idx_lsh[0]->query_points[q_loaded], &(rawfile[nodes[q_loaded][i].currentposition * data_dimensionality]), data_dimensionality);
            }
        } else {
            #pragma omp parallel for num_threads(num_thread)
            for (int i = 0; i < data_loaded; i++) {
                nodes[q_loaded][i].dist = faiss::fvec_L2sqr_avx512(idx_lsh[0]->query_points[q_loaded], &(rawfile[nodes[q_loaded][i].currentposition * data_dimensionality]), data_dimensionality);
            }
        }
        
        if (k_size < data_loaded) {
            std::partial_sort(nodes[q_loaded], nodes[q_loaded] + k_size, nodes[q_loaded] + data_loaded);
        } else {
            std::sort(nodes[q_loaded], nodes[q_loaded] + data_loaded);
        }
        gettimeofday(&end_phase_q, NULL);
        query_phase3 += (1000000 * (end_phase_q.tv_sec - start_phase_q.tv_sec) + end_phase_q.tv_usec - start_phase_q.tv_usec);

        gettimeofday(&end, NULL);
        query_all += (1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec);

        // std::cout << "Finish the " << q_loaded + 1 << "-th query." << std::endl;
        ++pd_query;
    }

    std::cout << "The indexing footprint is: " << memory_after_indexing - memory_before_indexing << "MB" << std::endl;
    std::cout << "The total time of encoding and indexing phase is: " << index_all / 1000.0 << "ms." << std::endl;
    std::cout << "The average query time is " << query_all / queries_size / 1000.0 << "ms." << std::endl;
    std::cout << "The average time of query phase1 is " << query_phase1 / queries_size / 1000.0 << "ms." << std::endl;
    std::cout << "The average time of query phase2 is " << query_phase2 / queries_size / 1000.0 << "ms." << std::endl;
    std::cout << "The average time of query phase3 is " << query_phase3 / queries_size / 1000.0 << "ms." << std::endl;

    std::cout << "-----------------Loading groundtruth-----------------" << std::endl;
    FILE *ifile_groundtruth;
    ifile_groundtruth = fopen (groundtruth,"rb");
    if (ifile_groundtruth == NULL) {
        fprintf(stderr, "File %s not found!\n", groundtruth);
        exit(-1);
    }
    long int ** groundtruth_result = (long int **) malloc(sizeof(long int*) * queries_size);
    for (int i = 0; i < queries_size; i++)
    {
        groundtruth_result[i] = (long int *) malloc(sizeof(long int) * k_size);
        fread(groundtruth_result[i], sizeof(long int), k_size, ifile_groundtruth);           
    }
    fclose(ifile_groundtruth);

    int retrived_data_num = 0;

    for (int i = 0; i < queries_size; i++)
    {
        int * obtained_result = new int[k_size];
        for (int j = 0; j < k_size; j++) {
            obtained_result[j] = nodes[i][j].currentposition;
        }

        for (int z = 0; z < k_size; z++) {
            for (int t = 0; t < k_size; t++) {
                if (obtained_result[z] == groundtruth_result[i][t]) {
                    retrived_data_num++;
                    break;
                }
            }
        }
    }

    float ratio = 0.0f;
    for (int i = 0; i < queries_size; i++)
    {
        for (int j = 0; j < k_size; j++)
        {
            float groundtruth_square_dist = euclidean_distance(idx_lsh[0]->query_points[i], &(rawfile[groundtruth_result[i][j] * data_dimensionality]), data_dimensionality);
            float obtained_square_dist = euclidean_distance(idx_lsh[0]->query_points[i], &(rawfile[nodes[i][j].currentposition * data_dimensionality]), data_dimensionality);
            if (groundtruth_square_dist == 0.0) {
                ratio += 1.0f;
            } else {
                ratio += sqrt(obtained_square_dist) / sqrt(groundtruth_square_dist);
            }
        }
    }

    float recall_value = float(retrived_data_num) / (queries_size*k_size);
    float overall_ratio = ratio / (queries_size * k_size);

    std::cout << "The average recall value is: " << recall_value << std::endl;
    std::cout << "The overall ratio is: " << overall_ratio << std::endl;

    return 0;
}
        
void INThandler(int sig)
{
    char  c;
    signal(sig, SIG_IGN);
    fprintf(stderr, "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y') {
    	exit(0);
    } else {
        signal(SIGINT, INThandler);
        getchar(); // Get new line character
    }  
}

void breakpoints_selection(isax_index *index, const char *ifilename, long int data_point_num, data_type * rawfile)
{
    int LSH_dimensionality = index->settings->LSH_dimensionality;
    unsigned int sample_size = index->settings->sample_size;
    
    fprintf(stderr, ">>> Sampling and selecting breakpoints from: %s\n", ifilename);
    COUNT_BINNING_TIME_START

    data_type ** sampled_data = (data_type **) calloc(LSH_dimensionality, sizeof(data_type*));
    for (int k = 0; k < LSH_dimensionality; ++k)
    {
        sampled_data[k] = (data_type *) calloc(sample_size, sizeof(data_type));
    }

    pthread_t threadid[num_thread];
    bins_data_inmemory *input_data=(bins_data_inmemory *)malloc(sizeof(bins_data_inmemory)*(num_thread));
    
    for (int i = 0; i < num_thread; i++)
    {
        input_data[i].index=index;
        input_data[i].sampled_data=sampled_data;
        input_data[i].filename=ifilename;
        input_data[i].workernumber=i;
        input_data[i].records = sample_size/num_thread;
        input_data[i].records_offset = sample_size/num_thread;

        //first-n-sampling
        if(index->settings->sample_type == 1)
        {
            input_data[i].start_number=i*(sample_size/num_thread);
            input_data[i].stop_number=(i+1)*(sample_size/num_thread);
        }
        //uniform sampling
        else if(index->settings->sample_type == 2)
        {
            input_data[i].start_number=i*(data_point_num/num_thread);
            input_data[i].stop_number=(i+1)*(data_point_num/num_thread);
        }
        //random sampling
        else if(index->settings->sample_type == 3)
        {
            input_data[i].start_number=0;
            input_data[i].stop_number=data_point_num;
        }
 
        input_data[i].raw_file = rawfile;

    }
 
    input_data[num_thread-1].records = sample_size - (num_thread-1)*(sample_size/num_thread);

    if(index->settings->sample_type == 1)
    {
        input_data[num_thread-1].stop_number=sample_size;
    } else if(index->settings->sample_type == 2)
    {
        input_data[num_thread-1].stop_number=data_point_num;
    }

    for (int i = 0; i < num_thread; i++)
    {
        pthread_create(&(threadid[i]),NULL,sampling_worker,(void*)&(input_data[i]));
    }

    for (int i = 0; i < num_thread; i++)
    {
        pthread_join(threadid[i],NULL);
    }
  
    for (int i = 0; i < num_thread; i++)
    {   
        input_data[i].start_number=i*(LSH_dimensionality/num_thread);
        input_data[i].stop_number=(i+1)*(LSH_dimensionality/num_thread);
    }

    input_data[num_thread-1].start_number=(num_thread-1)*(LSH_dimensionality/num_thread);
    input_data[num_thread-1].stop_number=LSH_dimensionality;

    for (int i = 0; i < num_thread; i++)
    {
        pthread_create(&(threadid[i]),NULL,breakpoints_selection_worker,(void*)&(input_data[i]));
    }

    for (int i = 0; i < num_thread; i++)
    {
        pthread_join(threadid[i],NULL);
    }

    free(input_data);
    free_sampled_data(index, sampled_data);

	fprintf(stderr, ">>> Finished sampling and selecting breakpoints.\n");
}

void* sampling_worker(void *transferdata)
{
    struct bins_data_inmemory* bins_data = (bins_data_inmemory*)transferdata;

    data_type ** sampled_data = bins_data->sampled_data;

    isax_index *index= ((bins_data_inmemory*)transferdata)->index;
    unsigned long start_number=bins_data->start_number;
    unsigned long stop_number=bins_data->stop_number;

    int LSH_dimensionality =  index->settings->LSH_dimensionality;

    unsigned long skip_elements;
    long records = bins_data->records;

    for(int i=0; i<records; ++i)
    {
        for (int j = 0; j < LSH_dimensionality; ++j)
        {
            if(index->settings->sample_type == 3)
            {
                sampled_data[j][i + (bins_data->workernumber * bins_data->records_offset)] = index->lsh_mem_array_for_init[j][start_number + random_at_most(records)];   
            } else {
                sampled_data[j][i + (bins_data->workernumber * bins_data->records_offset)] = index->lsh_mem_array_for_init[j][start_number + i]; 
            }
        }
    }
    return 0;
}

void* breakpoints_selection_worker(void *transferdata)
{
    struct bins_data_inmemory* bins_data = (bins_data_inmemory*)transferdata;

    data_type ** sampled_data = bins_data->sampled_data;

    isax_index *index= ((bins_data_inmemory*)transferdata)->index;
    unsigned long start_number=bins_data->start_number;
    unsigned long stop_number=bins_data->stop_number;

    unsigned int sample_size = index->settings->sample_size;
    int LSH_dimensionality =  index->settings->LSH_dimensionality;
    data_type min_value, max_value;
      
    for (int i = start_number; i < stop_number; i++)
    { 
        int select_round = log((double)index->settings->sax_alphabet_cardinality)/log(2.0);
        data_type * interval_lines = (data_type *) calloc(index->settings->sax_alphabet_cardinality-1, sizeof(data_type));
        int num_lines_to_insert, interval, num_intervals, start, end, index_interval, location; 
        for (int round = 1; round <= select_round; round++)
        {           
            num_intervals = pow(2, round);
            num_lines_to_insert = pow(2, round - 1);
            interval = sample_size / num_intervals;
            index_interval = index->settings->sax_alphabet_cardinality / num_intervals;
            location = 1;
            for (int j = 1; j <= num_lines_to_insert; j++)
            {
                start = (location-1) * interval;
                end = (location+1)*interval-1;
                if (j == num_lines_to_insert) {
                    end = sample_size - 1;
                }
                std::nth_element(sampled_data[i]+start, sampled_data[i] + interval, sampled_data[i] + end);
                interval_lines[location*index_interval-1] = sampled_data[i][start+interval];
                location += 2;
            }
        }
        for (int j = 0; j < index->settings->sax_alphabet_cardinality - 1; j++)
        {
            index->bins[i][j] = interval_lines[j];
        }

        min_value = *std::min_element(sampled_data[i], sampled_data[i] + interval);
        max_value = *std::max_element(sampled_data[i] + sample_size - 1 - interval, sampled_data[i] + sample_size - 1);

        index->bins_lower_bound[i] = min_value;
        index->bins_upper_bound[i] = max_value;
    }
    return 0;
}

void load_data(char * dataset, long int dataset_size, int data_dimensionality) {
    fprintf(stderr, ">>> Loading file: %s\n", dataset);

    FILE * ifiled;

    ifiled = fopen (dataset,"rb");

    if (ifiled == NULL) {
        fprintf(stderr, "File %s not found!\n", dataset);
        exit(-1);
    }

    fseek(ifiled, 0L, SEEK_END);
    file_position_type sz = (file_position_type) ftell(ifiled);
    file_position_type total_records = sz/data_dimensionality/sizeof(data_type);
    fseek(ifiled, 0L, SEEK_SET);

    if (total_records < dataset_size) {
        fprintf(stderr, "File %s has only %llu records!\n", dataset, total_records);
        exit(-1);
    }

    std::cout << "Cardinality of dataset is: " << dataset_size << std::endl;
    rawfile=(data_type*)malloc(sizeof(data_type) * data_dimensionality * dataset_size);
    int data_read_number=fread(rawfile, sizeof(data_type), data_dimensionality * dataset_size, ifiled);
    fclose(ifiled);
}

float euclidean_distance(data_type * t, data_type * s, int size) {
    float distance = 0;
    while (size > 0) {
        size--;
        distance += (t[size] - s[size]) * (t[size] - s[size]);
    }  
    return distance;
}

float euclidean_distance_SIMD(float * t, float * s, int size) {
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

    distancev = _mm256_hadd_ps (v_fd, v_fd);
    distancev = _mm256_hadd_ps (distancev, distancev);
    _mm256_storeu_ps (distancef ,distancev);
    distance +=distancef[0]+distancef[4];
    return distance;
}

namespace faiss {

// reads 0 <= d < 4 floats as __m128
static inline __m128 masked_read (int d, const float *x) {
    assert (0 <= d && d < 4);
    __attribute__((__aligned__(16))) float buf[4] = {0, 0, 0, 0};
    switch (d) {
        case 3:
            buf[2] = x[2];
        case 2:
            buf[1] = x[1];
        case 1:
            buf[0] = x[0];
    }
    return _mm_load_ps(buf);
    // cannot use AVX2 _mm_mask_set1_epi32
}
uint8_t lookup8bit[256];
//extern uint8_t lookup8bit[256];

float
fvec_inner_product_avx512(const float* x, const float* y, size_t d) {
    __m512 msum0 = _mm512_setzero_ps();

    while (d >= 16) {
        __m512 mx = _mm512_loadu_ps (x); x += 16;
        __m512 my = _mm512_loadu_ps (y); y += 16;
        msum0 = _mm512_add_ps (msum0, _mm512_mul_ps (mx, my));
        d -= 16;
    }

    __m256 msum1 = _mm512_extractf32x8_ps(msum0, 1);
    msum1 +=       _mm512_extractf32x8_ps(msum0, 0);

    if (d >= 8) {
        __m256 mx = _mm256_loadu_ps (x); x += 8;
        __m256 my = _mm256_loadu_ps (y); y += 8;
        msum1 = _mm256_add_ps (msum1, _mm256_mul_ps (mx, my));
        d -= 8;
    }

    __m128 msum2 = _mm256_extractf128_ps(msum1, 1);
    msum2 +=       _mm256_extractf128_ps(msum1, 0);

    if (d >= 4) {
        __m128 mx = _mm_loadu_ps (x); x += 4;
        __m128 my = _mm_loadu_ps (y); y += 4;
        msum2 = _mm_add_ps (msum2, _mm_mul_ps (mx, my));
        d -= 4;
    }

    if (d > 0) {
        __m128 mx = masked_read (d, x);
        __m128 my = masked_read (d, y);
        msum2 = _mm_add_ps (msum2, _mm_mul_ps (mx, my));
    }

    msum2 = _mm_hadd_ps (msum2, msum2);
    msum2 = _mm_hadd_ps (msum2, msum2);
    return  _mm_cvtss_f32 (msum2);
}

float
fvec_L2sqr_avx512(const float* x, const float* y, size_t d) {
    __m512 msum0 = _mm512_setzero_ps();

    while (d >= 16) {
        __m512 mx = _mm512_loadu_ps (x); x += 16;
        __m512 my = _mm512_loadu_ps (y); y += 16;
        const __m512 a_m_b1 = mx - my;
        msum0 += a_m_b1 * a_m_b1;
        d -= 16;
    }

    __m256 msum1 = _mm512_extractf32x8_ps(msum0, 1);
    msum1 +=       _mm512_extractf32x8_ps(msum0, 0);

    if (d >= 8) {
        __m256 mx = _mm256_loadu_ps (x); x += 8;
        __m256 my = _mm256_loadu_ps (y); y += 8;
        const __m256 a_m_b1 = mx - my;
        msum1 += a_m_b1 * a_m_b1;
        d -= 8;
    }

    __m128 msum2 = _mm256_extractf128_ps(msum1, 1);
    msum2 +=       _mm256_extractf128_ps(msum1, 0);

    if (d >= 4) {
        __m128 mx = _mm_loadu_ps (x); x += 4;
        __m128 my = _mm_loadu_ps (y); y += 4;
        const __m128 a_m_b1 = mx - my;
        msum2 += a_m_b1 * a_m_b1;
        d -= 4;
    }

    if (d > 0) {
        __m128 mx = masked_read (d, x);
        __m128 my = masked_read (d, y);
        __m128 a_m_b1 = mx - my;
        msum2 += a_m_b1 * a_m_b1;
    }

    msum2 = _mm_hadd_ps (msum2, msum2);
    msum2 = _mm_hadd_ps (msum2, msum2);
    return  _mm_cvtss_f32 (msum2);
}

float
fvec_L1_avx512(const float* x, const float* y, size_t d) {
    __m512 msum0 = _mm512_setzero_ps();
    __m512 signmask0 = __m512(_mm512_set1_epi32 (0x7fffffffUL));

    while (d >= 16) {
        __m512 mx = _mm512_loadu_ps (x); x += 16;
        __m512 my = _mm512_loadu_ps (y); y += 16;
        const __m512 a_m_b = mx - my;
        msum0 += _mm512_and_ps(signmask0, a_m_b);
        d -= 16;
    }

    __m256 msum1 = _mm512_extractf32x8_ps(msum0, 1);
    msum1 +=       _mm512_extractf32x8_ps(msum0, 0);
    __m256 signmask1 = __m256(_mm256_set1_epi32 (0x7fffffffUL));

    if (d >= 8) {
        __m256 mx = _mm256_loadu_ps (x); x += 8;
        __m256 my = _mm256_loadu_ps (y); y += 8;
        const __m256 a_m_b = mx - my;
        msum1 += _mm256_and_ps(signmask1, a_m_b);
        d -= 8;
    }

    __m128 msum2 = _mm256_extractf128_ps(msum1, 1);
    msum2 +=       _mm256_extractf128_ps(msum1, 0);
    __m128 signmask2 = __m128(_mm_set1_epi32 (0x7fffffffUL));

    if (d >= 4) {
        __m128 mx = _mm_loadu_ps (x); x += 4;
        __m128 my = _mm_loadu_ps (y); y += 4;
        const __m128 a_m_b = mx - my;
        msum2 += _mm_and_ps(signmask2, a_m_b);
        d -= 4;
    }

    if (d > 0) {
        __m128 mx = masked_read (d, x);
        __m128 my = masked_read (d, y);
        __m128 a_m_b = mx - my;
        msum2 += _mm_and_ps(signmask2, a_m_b);
    }

    msum2 = _mm_hadd_ps (msum2, msum2);
    msum2 = _mm_hadd_ps (msum2, msum2);
    return  _mm_cvtss_f32 (msum2);
}

float
fvec_Linf_avx512(const float* x, const float* y, size_t d) {
    __m512 msum0 = _mm512_setzero_ps();
    __m512 signmask0 = __m512(_mm512_set1_epi32 (0x7fffffffUL));

    while (d >= 16) {
        __m512 mx = _mm512_loadu_ps (x); x += 16;
        __m512 my = _mm512_loadu_ps (y); y += 16;
        const __m512 a_m_b = mx - my;
        msum0 = _mm512_max_ps(msum0, _mm512_and_ps(signmask0, a_m_b));
        d -= 16;
    }

    __m256 msum1 = _mm512_extractf32x8_ps(msum0, 1);
    msum1 = _mm256_max_ps (msum1, _mm512_extractf32x8_ps(msum0, 0));
    __m256 signmask1 = __m256(_mm256_set1_epi32 (0x7fffffffUL));

    if (d >= 8) {
        __m256 mx = _mm256_loadu_ps (x); x += 8;
        __m256 my = _mm256_loadu_ps (y); y += 8;
        const __m256 a_m_b = mx - my;
        msum1 = _mm256_max_ps(msum1, _mm256_and_ps(signmask1, a_m_b));
        d -= 8;
    }

    __m128 msum2 = _mm256_extractf128_ps(msum1, 1);
    msum2 = _mm_max_ps (msum2, _mm256_extractf128_ps(msum1, 0));
    __m128 signmask2 = __m128(_mm_set1_epi32 (0x7fffffffUL));

    if (d >= 4) {
        __m128 mx = _mm_loadu_ps (x); x += 4;
        __m128 my = _mm_loadu_ps (y); y += 4;
        const __m128 a_m_b = mx - my;
        msum2 = _mm_max_ps(msum2, _mm_and_ps(signmask2, a_m_b));
        d -= 4;
    }

    if (d > 0) {
        __m128 mx = masked_read (d, x);
        __m128 my = masked_read (d, y);
        __m128 a_m_b = mx - my;
        msum2 = _mm_max_ps(msum2, _mm_and_ps(signmask2, a_m_b));
    }

    msum2 = _mm_max_ps(_mm_movehl_ps(msum2, msum2), msum2);
    msum2 = _mm_max_ps(msum2, _mm_shuffle_ps (msum2, msum2, 1));
    return  _mm_cvtss_f32 (msum2);
}

uint64_t
_mm256_hsum_epi64(__m256i v) {
    return _mm256_extract_epi64(v, 0) +
           _mm256_extract_epi64(v, 1) +
           _mm256_extract_epi64(v, 2) +
           _mm256_extract_epi64(v, 3);
}

uint64_t _mm512_hsum_epi64(__m512i v) {
    const __m256i t0 = _mm512_extracti64x4_epi64(v, 0);
    const __m256i t1 = _mm512_extracti64x4_epi64(v, 1);

    return _mm256_hsum_epi64(t0) + _mm256_hsum_epi64(t1);
}

int
popcnt_AVX512VBMI_lookup(const uint8_t* data, const size_t n) {

    size_t i = 0;

    const __m512i lookup = _mm512_setr_epi64(
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu
    );

    const __m512i low_mask = _mm512_set1_epi8(0x0f);

    __m512i acc = _mm512_setzero_si512();

    while (i + 64 < n) {

        __m512i local = _mm512_setzero_si512();

        for (int k=0; k < 255/8 && i + 64 < n; k++, i += 64) {
            const __m512i vec = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data + i));
            const __m512i lo  = _mm512_and_si512(vec, low_mask);
            const __m512i hi  = _mm512_and_si512(_mm512_srli_epi32(vec, 4), low_mask);

            const __m512i popcnt1 = _mm512_shuffle_epi8(lookup, lo);
            const __m512i popcnt2 = _mm512_shuffle_epi8(lookup, hi);

            local = _mm512_add_epi8(local, popcnt1);
            local = _mm512_add_epi8(local, popcnt2);
        }

        acc = _mm512_add_epi64(acc, _mm512_sad_epu8(local, _mm512_setzero_si512()));
    }


    int result = _mm512_hsum_epi64(acc);
    for (/**/; i < n; i++) {
        result += lookup8bit[data[i]];
    }

    return result;
}

int
xor_popcnt_AVX512VBMI_lookup(const uint8_t* data1, const uint8_t* data2, const size_t n) {

    size_t i = 0;

    const __m512i lookup = _mm512_setr_epi64(
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu
    );

    const __m512i low_mask = _mm512_set1_epi8(0x0f);

    __m512i acc = _mm512_setzero_si512();

    while (i + 64 < n) {

        __m512i local = _mm512_setzero_si512();

        for (int k=0; k < 255/8 && i + 64 < n; k++, i += 64) {
            const __m512i s1 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data1 + i));
            const __m512i s2 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data2 + i));
            const __m512i vec = _mm512_xor_si512(s1, s2);
            const __m512i lo  = _mm512_and_si512(vec, low_mask);
            const __m512i hi  = _mm512_and_si512(_mm512_srli_epi32(vec, 4), low_mask);

            const __m512i popcnt1 = _mm512_shuffle_epi8(lookup, lo);
            const __m512i popcnt2 = _mm512_shuffle_epi8(lookup, hi);

            local = _mm512_add_epi8(local, popcnt1);
            local = _mm512_add_epi8(local, popcnt2);
        }

        acc = _mm512_add_epi64(acc, _mm512_sad_epu8(local, _mm512_setzero_si512()));
    }


    int result = _mm512_hsum_epi64(acc);
    for (/**/; i < n; i++) {
        result += lookup8bit[data1[i]^data2[i]];
    }

    return result;
}

int
or_popcnt_AVX512VBMI_lookup(const uint8_t* data1, const uint8_t* data2, const size_t n) {

    size_t i = 0;

    const __m512i lookup = _mm512_setr_epi64(
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu
    );

    const __m512i low_mask = _mm512_set1_epi8(0x0f);

    __m512i acc = _mm512_setzero_si512();

    while (i + 64 < n) {

        __m512i local = _mm512_setzero_si512();

        for (int k=0; k < 255/8 && i + 64 < n; k++, i += 64) {
            const __m512i s1 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data1 + i));
            const __m512i s2 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data2 + i));
            const __m512i vec = _mm512_or_si512(s1, s2);
            const __m512i lo  = _mm512_and_si512(vec, low_mask);
            const __m512i hi  = _mm512_and_si512(_mm512_srli_epi32(vec, 4), low_mask);

            const __m512i popcnt1 = _mm512_shuffle_epi8(lookup, lo);
            const __m512i popcnt2 = _mm512_shuffle_epi8(lookup, hi);

            local = _mm512_add_epi8(local, popcnt1);
            local = _mm512_add_epi8(local, popcnt2);
        }

        acc = _mm512_add_epi64(acc, _mm512_sad_epu8(local, _mm512_setzero_si512()));
    }


    int result = _mm512_hsum_epi64(acc);
    for (/**/; i < n; i++) {
        result += lookup8bit[data1[i]|data2[i]];
    }

    return result;
}

int
and_popcnt_AVX512VBMI_lookup(const uint8_t* data1, const uint8_t* data2, const size_t n) {

    size_t i = 0;

    const __m512i lookup = _mm512_setr_epi64(
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu,
        0x0302020102010100llu, 0x0403030203020201llu
    );

    const __m512i low_mask = _mm512_set1_epi8(0x0f);

    __m512i acc = _mm512_setzero_si512();

    while (i + 64 < n) {

        __m512i local = _mm512_setzero_si512();

        for (int k=0; k < 255/8 && i + 64 < n; k++, i += 64) {
            const __m512i s1 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data1 + i));
            const __m512i s2 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data2 + i));
            const __m512i vec = _mm512_and_si512(s1, s2);
            const __m512i lo  = _mm512_and_si512(vec, low_mask);
            const __m512i hi  = _mm512_and_si512(_mm512_srli_epi32(vec, 4), low_mask);

            const __m512i popcnt1 = _mm512_shuffle_epi8(lookup, lo);
            const __m512i popcnt2 = _mm512_shuffle_epi8(lookup, hi);

            local = _mm512_add_epi8(local, popcnt1);
            local = _mm512_add_epi8(local, popcnt2);
        }

        acc = _mm512_add_epi64(acc, _mm512_sad_epu8(local, _mm512_setzero_si512()));
    }


    int result = _mm512_hsum_epi64(acc);
    for (/**/; i < n; i++) {
        result += lookup8bit[data1[i]&data2[i]];
    }

    return result;
}

float
jaccard_AVX512(const uint8_t * a, const uint8_t * b, size_t n) {
    int accu_num = and_popcnt_AVX512VBMI_lookup(a,b,n);
    int accu_den = or_popcnt_AVX512VBMI_lookup(a,b,n);
    return (accu_den == 0) ? 1.0 : ((float)(accu_den - accu_num) / (float)(accu_den));
}

} // namespace faiss
