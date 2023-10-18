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
    static long int dataset_size = 6000000;//testbench
    static int queries_size = 10;
    static int data_dimensionality = 256;
    static int LSH_dimensionality = 16;
    static int sax_cardinality = 8;
    static int leaf_size = 2000;
    static int min_leaf_size = 10;
    static int initial_lbl_size = 2000;
    static int flush_limit = 300000;
    static int initial_fbl_size = 100;
    static int complete_type = 0;
    static int total_loaded_leaves = 1;
    static int tight_bound = 0;
    static int aggressive_check = 0;
    static int cpu_control_type = 1;
    static char SIMD_flag=0;
    static int sample_type=2;
    static int generate_groundtruth=0;

    int calculate_thread=8;
    l_size =4;
    N_PQUEUE =1;
    maxreadthread=5;
    int k_size=50;
    float search_radius=1;
    int max_candidate_size=1;
    int sample_size=1;

    time_t time_now;

    while (1)
    {
        static struct option long_options[] =  {
            {"initial-lbl-size", required_argument, 0, 'b'},
            {"complete-type", required_argument, 0, 'c'},
            {"dataset", required_argument, 0, 'd'},
            {"total-loaded-leaves", required_argument, 0, 'e'},
            {"flush-limit", required_argument, 0, 'f'},
            {"aggressive-check", no_argument, 0, 'g'},
            {"help", no_argument, 0, 'h'},
            {"initial-fbl-size", required_argument, 0, 'i'},
            {"queries-size", required_argument, 0, 'k'},
            {"leaf-size", required_argument, 0, 'l'},
            {"min-leaf-size", required_argument, 0, 'm'},
            {"tight-bound", no_argument, 0, 'n'},
            {"read-thread", required_argument, 0, 'o'},
            {"index-path", required_argument, 0, 'p'},
            {"queries", required_argument, 0, 'q'},
            {"read-block", required_argument, 0, 'r'},
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
            {"generate-groundtruth", no_argument, 0, 'I'},
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
            case 'g':
                aggressive_check = 1;
                break;
            case 'n':
                tight_bound = 1;
                break;

            case 'e':
                total_loaded_leaves = atoi(optarg);
                break;

            case 'c':
                complete_type = atoi(optarg);
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
            case 'o':
                maxreadthread = atoi(optarg);
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
                search_radius = atoi(optarg);
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
                \t--minimum-distance XX\t\tThe minimum distance we search (MAX if not set)\n\
                \t--index-path XX \t\tThe path of the output folder\n\
                \t--data-dimensionality XX\t\tThe size of each data point\n\
                \t--sax-cardinality XX\t\tThe maximum sax cardinality in number of bits (power of two).\n\
                \t--LSH-dimensionality XX\t\tThe dimensionality of each projected space.\n\
                \t--leaf-size XX\t\t\tThe maximum size of each leaf\n\
                \t--min-leaf-size XX\t\tThe minimum size of each leaf\n\
                \t--initial-lbl-size XX\t\tThe initial lbl buffer size for each buffer.\n\
                \t--flush-limit XX\t\tThe limit of time series in memory at the same time\n\
                \t--initial-fbl-size XX\t\tThe initial fbl buffer size for each buffer.\n\
                \t--complete-type XX\t\t0 for no complete, 1 for serial, 2 for leaf\n\
                \t--total-loaded-leaves XX\tNumber of leaves to load at each fetch\n\
                \t--min-checked-leaves XX\t\tNumber of leaves to check at minimum\n\
                \t--tight-bound XX\tSet for tight bounds.\n\
                \t--aggressive-check XX\t\tSet for aggressive check\n\
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
            case 'I':
                generate_groundtruth = 1;
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

    if(cpu_control_type==21)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        calculate_thread=2;
        maxquerythread=2;
    }
    else if(cpu_control_type==22)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        calculate_thread=2;
        maxquerythread=2;
    }
    else if(cpu_control_type==41)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
	    calculate_thread=4;
        maxquerythread=4;
    }
    else if(cpu_control_type==42)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        calculate_thread=4;
        maxquerythread=4;
    }
    else if(cpu_control_type==61)
    {
        CPU_SET(0, &mask);
        CPU_SET(2, &mask);
        CPU_SET(4, &mask);
        CPU_SET(6, &mask);
        CPU_SET(8, &mask);
        CPU_SET(10, &mask);
        calculate_thread=6;
        maxquerythread=6;
    }
    else if(cpu_control_type==62)
    {
        CPU_SET(0, &mask);
        CPU_SET(1, &mask);
        CPU_SET(2, &mask);
        CPU_SET(3, &mask);
        CPU_SET(4, &mask);
        CPU_SET(5, &mask);
        calculate_thread=6;
        maxquerythread=6;
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
        calculate_thread=8;
        maxquerythread=8;
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
        calculate_thread=8;
        maxquerythread=8;
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
        calculate_thread=10;
        maxquerythread=10;
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
        calculate_thread=10;
        maxquerythread=10;
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
        calculate_thread=12;
        maxquerythread=12;
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
        calculate_thread=12;
        maxquerythread=12;
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
            calculate_thread=18;
            maxquerythread=18;
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
            calculate_thread=24;
            maxquerythread=24;
    }
    //new control type for gruenau1-server with 36 cores on 2 CPUs
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
            calculate_thread=36;
            maxquerythread=36;
    }
    else if(cpu_control_type==1)
    {

        calculate_thread=1;
        maxquerythread=1;
    }
    else
    {
        calculate_thread=cpu_control_type;
        maxquerythread=cpu_control_type;

        for (int i = 0; i < cpu_control_type; i++)
        {
            //CPU_SET(i, &mask);
        }
    }


    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) 
    {
        fprintf(stderr, "set thread affinity failed\n");
    }

    if (pthread_getaffinity_np(pthread_self(), sizeof(get), &get) < 0) 
    {
        fprintf(stderr, "get thread affinity failed\n");
    }
    char rm_command[256];

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
                                                                        tight_bound,		// Tightness of leaf bounds
                                                                        aggressive_check,	// aggressive check
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
    std::normal_distribution<float> nd;

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::chrono::duration<float> duration, query_all, index_all;

    // Load data
    load_data(dataset, dataset_size);

    // Encoding and indexing
    for (int num_l = 0; num_l < l_size; num_l++)
    {
        std::cout << "Generating LSH vectors and calculating the LSH representation for all data in the " << num_l << "th index tree." << std::endl;
        // Initialize LSH vectors
        idx_lsh[num_l]->lsh_hash_set = (data_type **) calloc(idx_lsh[num_l]->settings->LSH_dimensionality, sizeof(data_type*));
        for (int i = 0; i < idx_lsh[num_l]->settings->LSH_dimensionality; i++)
        {
            idx_lsh[num_l]->lsh_hash_set[i] = (data_type *) calloc(idx_lsh[num_l]->settings->data_dimensionality, sizeof(data_type));
            
            for(int j = 0; j < idx_lsh[num_l]->settings->data_dimensionality; j++)
            {
                idx_lsh[num_l]->lsh_hash_set[i][j] = (data_type) (nd(rng));
            }
        }

        // Generate LSH representations for all points
        idx_lsh[num_l]->lsh_mem_array = (data_type **) calloc(dataset_size, sizeof(data_type*));
        idx_lsh[num_l]->lsh_mem_array_for_init = (data_type **) calloc(idx_lsh[num_l]->settings->LSH_dimensionality, sizeof(data_type*));
        data_type * data_point = (data_type *) malloc(sizeof(data_type) * idx_lsh[num_l]->settings->data_dimensionality);

        for (int i = 0; i < idx_lsh[num_l]->settings->LSH_dimensionality; i++)
        {
            idx_lsh[num_l]->lsh_mem_array_for_init[i] = (data_type *) calloc(dataset_size, sizeof(data_type));
        }

        for (int i = 0; i < dataset_size; i++)
        {
            memcpy(data_point,&(rawfile[i*idx_lsh[num_l]->settings->data_dimensionality]), sizeof(data_type)* idx_lsh[num_l]->settings->data_dimensionality);
            idx_lsh[num_l]->lsh_mem_array[i] = (data_type *) calloc(idx_lsh[num_l]->settings->LSH_dimensionality, sizeof(data_type));                  
            for(int j = 0; j < idx_lsh[num_l]->settings->LSH_dimensionality; j++)
            {
                data_type value = 0.0;
                for(int k = 0; k < idx_lsh[num_l]->settings->data_dimensionality; k++)
                {
                    value += idx_lsh[num_l]->lsh_hash_set[j][k] * data_point[k];
                }
                idx_lsh[num_l]->lsh_mem_array[i][j] = value;
                idx_lsh[num_l]->lsh_mem_array_for_init[j][i] = value;
            }
        }

        // Breakpoints selection
        start = std::chrono::high_resolution_clock::now();
        breakpoints_init(idx_lsh[num_l]);
        breakpoints_selection(idx_lsh[num_l], dataset, dataset_size, maxquerythread, rawfile);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        index_all += duration;

        // Free memory for breakpoints selection
        for (int i = 0; i < idx_lsh[num_l]->settings->LSH_dimensionality; i++)
        {
            free(idx_lsh[num_l]->lsh_mem_array_for_init[i]);
        }
        free(idx_lsh[num_l]->lsh_mem_array_for_init);
       
        // Encoding and constructing indexes
        start = std::chrono::high_resolution_clock::now();
        index_creation(dataset_size, idx_lsh[num_l]);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        index_all += duration;

        // Free memory for encoding and indexing
        for (int i = 0; i < dataset_size; i++)
        {
            free(idx_lsh[num_l]->lsh_mem_array[i]);
        }
        free(idx_lsh[num_l]->lsh_mem_array);  
    }

    // Load and generate LSH representations for queries
    std::cout << "Load and generate LSH representations for queries." << std::endl;
    FILE *ifile;
    ifile = fopen (queries,"rb");

    if (ifile == NULL) {
        fprintf(stderr, "File %s not found!\n", queries);
        exit(-1);
    }

    fseek(ifile, 0L, SEEK_END);
    file_position_type sz = (file_position_type) ftell(ifile);
    file_position_type total_records = sz/idx_lsh[0]->settings->point_byte_size;
    fseek(ifile, 0L, SEEK_SET);

    if (total_records < queries_size) {
        fprintf(stderr, "File %s has only %llu records!\n", queries, total_records);
        exit(-1);
    }

    data_type* queryfile;
    queryfile = (data_type*)malloc(sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality * queries_size);
    int query_read_number=fread(queryfile, sizeof(data_type), idx_lsh[0]->settings->data_dimensionality*queries_size, ifile);

    data_type * querypoint = (data_type *)malloc(sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality);
    candidate_node ** nodes = (candidate_node **)malloc(sizeof(candidate_node*) * queries_size);

    for (int i = 0; i < l_size; i++)
    {
        idx_lsh[i]->lsh_mem_array_for_query = (data_type **) calloc(queries_size, sizeof(data_type*));
        idx_lsh[i]->query_points = (data_type **) calloc(queries_size, sizeof(data_type*));
        for (int j = 0; j < queries_size; j++)
        {
            idx_lsh[i]->lsh_mem_array_for_query[j] = (data_type *) calloc(idx_lsh[i]->settings->LSH_dimensionality, sizeof(data_type));
            idx_lsh[i]->query_points[j] = (data_type *) calloc(idx_lsh[i]->settings->data_dimensionality, sizeof(data_type));
        }
    }

    for (int q_loaded = 0; q_loaded < queries_size; q_loaded++)
    {
        memcpy(querypoint,&(queryfile[q_loaded*idx_lsh[0]->settings->data_dimensionality]), sizeof(data_type)* idx_lsh[0]->settings->data_dimensionality);
        for (int i = 0; i < l_size; i++)
        {                                      
            for(int j = 0; j < idx_lsh[i]->settings->LSH_dimensionality; j++)
            {
                data_type value = 0.0;
                for(int k = 0; k < idx_lsh[i]->settings->data_dimensionality; k++)
                {
                    idx_lsh[i]->query_points[q_loaded][k] = querypoint[k];
                    value += idx_lsh[i]->lsh_hash_set[j][k] * querypoint[k];
                }
                idx_lsh[i]->lsh_mem_array_for_query[q_loaded][j] = value;
            }  
        }                             
    } 

    fclose(ifile);
    
    std::cout << "-----------------Start query-----------------" << std::endl;
    // The first step of query: generate the priority queue of candidate leaf nodes for each query
    start = std::chrono::high_resolution_clock::now();
    for (int num_l = 0; num_l < l_size; num_l++)
    {     
        range_query(queries, queries_size, idx_lsh[num_l], k_size, idx_lsh[num_l]->lsh_hash_set, search_radius, &range_search_lsh);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    query_all += duration;

    // Perform subsequent steps for each query
    for (int q_loaded = 0; q_loaded < queries_size; ++q_loaded)
    {
        // The second step of query: generate the candidate set of points by obtained priority queues
        start = std::chrono::high_resolution_clock::now();

        float localannbsf=FLT_MAX;
        long int currentposition=0;   
        int data_loaded = 0; 

        std::unordered_map<long int, bool> isCandidate;

        nodes[q_loaded] = (candidate_node *)malloc(sizeof(candidate_node) * max_candidate_size);

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
            float dist, dist_euclidean;

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
                    dist_euclidean = euclidean_distance(idx_lsh[mindist_index]->query_points[q_loaded], &(rawfile[*index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_position_buffer[i]]), 
                                        idx_lsh[0]->settings->data_dimensionality);
                    nodes[q_loaded][data_loaded].dist = dist_euclidean;
                    nodes[q_loaded][data_loaded].currentposition = *index_query_results[mindist_index].result[num_queue[mindist_index]]->node->buffer->partial_position_buffer[i] / idx_lsh[0]->settings->data_dimensionality;                      
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

        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        query_all += duration;

        // The third step of query: obtain the k-NN from the points in the candidate set
        start = std::chrono::high_resolution_clock::now();
        if (k_size < data_loaded) {
            std::partial_sort(nodes[q_loaded], nodes[q_loaded] + k_size, nodes[q_loaded] + data_loaded);
        } else {
            std::sort(nodes[q_loaded], nodes[q_loaded] + data_loaded);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        query_all += duration;

        std::cout << "Finish the " << q_loaded + 1 << "-th query." << std::endl;
    }

    std::cout << "The total time of encoding and indexing phase is: " << index_all.count() * 1000.0f << "ms." << std::endl;
    std::cout << "The average query time is " << query_all.count() * 1000.0f / queries_size << "ms." << std::endl;

    if (generate_groundtruth)
    {
        std::cout << "-----------------Generating groundtruth-----------------" << std::endl;

        data_type ** result = (data_type **) malloc(sizeof(data_type*) * queries_size);
        for (int i = 0; i < queries_size; i++)
        {
            std::cout << "For the " << i << "-th query." << std::endl;
            result[i] = (data_type *) malloc(sizeof(data_type) * k_size);
            data_type * querypoint = (data_type *) malloc(sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality);
            memcpy(querypoint,&(queryfile[i*idx_lsh[0]->settings->data_dimensionality]), sizeof(data_type)* idx_lsh[0]->settings->data_dimensionality);
            data_type * distance = (data_type *) malloc(sizeof(data_type) * dataset_size);
            for (int j = 0; j < dataset_size; j++)
            {
                data_type * datapoint = (data_type *) malloc(sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality);
                memcpy(datapoint,&(rawfile[j*idx_lsh[0]->settings->data_dimensionality]), sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality);
                distance[j] = euclidean_distance(querypoint, datapoint, idx_lsh[0]->settings->data_dimensionality);
                free(datapoint);
            }
            std::partial_sort(distance, distance + k_size, distance + dataset_size);
            for (int k = 0; k < k_size; k++)
            {
                result[i][k] = distance[k];
            }
            free(querypoint);
            free(distance);
        }

        int retrived_data_num = 0;

        for (int i = 0; i < queries_size; i++)
        {
            for (int j = k_size - 1; j >= 0; j--)
            {
                if (nodes[i][j].dist <= result[i][k_size-1]) {
                    retrived_data_num += (j+1);
                    break;
                }
            }
        }

        float ratio = 0.0f;
        for (int i = 0; i < queries_size; i++)
        {
            for (int j = 0; j < k_size; j++)
            {
                if (result[i][j] == 0) {
                    ratio += 1.0f;
                } else {
                    ratio += sqrt(nodes[i][j].dist) / sqrt(result[i][j]);
                }
            }
        }

        float recall_value = float(retrived_data_num) / (queries_size*k_size);
        float overall_ratio = ratio / (queries_size * k_size);

        std::cout << "The average recall value is: " << recall_value << std::endl;
        std::cout << "The overall ratio is: " << overall_ratio << std::endl;

        std::cout << "-----------------Saving groundtruth-----------------" << std::endl;
        FILE *ifile_groundtruth;
        ifile_groundtruth = fopen (groundtruth,"wb");
        for (int i = 0; i < queries_size; i++)
        {
            fwrite(result[i], sizeof(float), k_size, ifile_groundtruth);            
        }
        fclose(ifile_groundtruth);
    } else {
        std::cout << "-----------------Loading groundtruth-----------------" << std::endl;
        FILE *ifile_groundtruth;
        ifile_groundtruth = fopen (groundtruth,"rb");
        if (ifile_groundtruth == NULL) {
            fprintf(stderr, "File %s not found!\n", groundtruth);
            exit(-1);
        }
        data_type ** result = (data_type **) malloc(sizeof(data_type *) * queries_size);
        for (int i = 0; i < queries_size; i++)
        {
            result[i] = (data_type *) malloc(sizeof(data_type) * k_size);
            fread(result[i], sizeof(float), k_size, ifile_groundtruth);                
        }

        int retrived_data_num = 0;

        for (int i = 0; i < queries_size; i++)
        {
            for (int j = k_size - 1; j >= 0; j--)
            {
                if (nodes[i][j].dist <= result[i][k_size-1]) {
                    retrived_data_num += (j+1);
                    break;
                }
            }
        }

        float ratio = 0.0f;
        for (int i = 0; i < queries_size; i++)
        {
            for (int j = 0; j < k_size; j++)
            {
                if (result[i][j] == 0) {
                    ratio += 1.0f;
                } else {
                    ratio += sqrt(nodes[i][j].dist) / sqrt(result[i][j]);
                }
            }
        }

        float recall_value = float(retrived_data_num) / (queries_size*k_size);
        float overall_ratio = ratio / (queries_size * k_size);

        std::cout << "The average recall value is: " << recall_value << std::endl;
        std::cout << "The overall ratio is: " << overall_ratio << std::endl;
    }

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

void breakpoints_selection(isax_index *index, const char *ifilename, long int ts_num, int maxquerythread, data_type * rawfile)
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

    pthread_t threadid[maxquerythread];
    bins_data_inmemory *input_data=(bins_data_inmemory *)malloc(sizeof(bins_data_inmemory)*(maxquerythread));
    
    for (int i = 0; i < maxquerythread; i++)
    {
        input_data[i].index=index;
        input_data[i].sampled_data=sampled_data;
        input_data[i].filename=ifilename;
        input_data[i].workernumber=i;
        input_data[i].records = sample_size/maxquerythread;
        input_data[i].records_offset = sample_size/maxquerythread;

        //first-n-sampling
        if(index->settings->sample_type == 1)
        {
            input_data[i].start_number=i*(sample_size/maxquerythread);
            input_data[i].stop_number=(i+1)*(sample_size/maxquerythread);
        }
        //uniform sampling
        else if(index->settings->sample_type == 2)
        {
            input_data[i].start_number=i*(ts_num/maxquerythread);
            input_data[i].stop_number=(i+1)*(ts_num/maxquerythread);
        }
        //random sampling
        else if(index->settings->sample_type == 3)
        {
            input_data[i].start_number=0;
            input_data[i].stop_number=ts_num;
        }
 
        input_data[i].raw_file = rawfile;

    }
 
    input_data[maxquerythread-1].records = sample_size - (maxquerythread-1)*(sample_size/maxquerythread);

    if(index->settings->sample_type == 1)
    {
        input_data[maxquerythread-1].stop_number=sample_size;
    } else if(index->settings->sample_type == 2)
    {
        input_data[maxquerythread-1].stop_number=ts_num;
    }

    for (int i = 0; i < maxquerythread; i++)
    {
        pthread_create(&(threadid[i]),NULL,sampling_worker,(void*)&(input_data[i]));
    }

    for (int i = 0; i < maxquerythread; i++)
    {
        pthread_join(threadid[i],NULL);
    }
  
    for (int i = 0; i < maxquerythread; i++)
    {   
        input_data[i].start_number=i*(LSH_dimensionality/maxquerythread);
        input_data[i].stop_number=(i+1)*(LSH_dimensionality/maxquerythread);
    }

    input_data[maxquerythread-1].start_number=(maxquerythread-1)*(LSH_dimensionality/maxquerythread);
    input_data[maxquerythread-1].stop_number=LSH_dimensionality;

    for (int i = 0; i < maxquerythread; i++)
    {
        pthread_create(&(threadid[i]),NULL,breakpoints_selection_worker,(void*)&(input_data[i]));
    }

    for (int i = 0; i < maxquerythread; i++)
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

void load_data(char * dataset, long int dataset_size) {
    fprintf(stderr, ">>> Loading file: %s\n", dataset);

    FILE * ifiled;

    ifiled = fopen (dataset,"rb");

    if (ifiled == NULL) {
        fprintf(stderr, "File %s not found!\n", dataset);
        exit(-1);
    }

    fseek(ifiled, 0L, SEEK_END);
    file_position_type sz = (file_position_type) ftell(ifiled);
    file_position_type total_records = sz/idx_lsh[0]->settings->data_dimensionality/sizeof(data_type);
    fseek(ifiled, 0L, SEEK_SET);

    if (total_records < dataset_size) {
        fprintf(stderr, "File %s has only %llu records!\n", dataset, total_records);
        exit(-1);
    }

    std::cout << "Cardinality of dataset is: " << dataset_size << std::endl;
    rawfile=(data_type*)malloc(sizeof(data_type) * idx_lsh[0]->settings->data_dimensionality*dataset_size);
    int data_read_number=fread(rawfile, sizeof(data_type), idx_lsh[0]->settings->data_dimensionality*dataset_size, ifiled);
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