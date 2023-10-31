# DET-LSH: A Locality-Sensitive Hashing Scheme with Dynamic Encoding Tree for Approximate Nearest Neighbor Search

This is the source code of the method proposed in paper: **DET-LSH: A Locality-Sensitive Hashing Scheme with Dynamic Encoding Tree for Approximate Nearest Neighbor Search (submitted to VLDB 2024)**.

## Compilation

```
./configure
make
```

## Usage

### Command and Parameters

```
cd ./bin
./DETLSH --dataset $PATH_TO_DATASET$ --leaf-size $MAX_SIZE$ --dataset-size $n$  --queries $PATH_TO_QUERY$ --queries-size $QUERY_SIZE$ --sample-size $SAMPLE_SIZE$ --sample-type $SAMPLE_TYPE$ --k-size $k$ --data-dimensionality $DIMENSIONALITY$ --l-size $L$ --search-radius $r$ --max-candidate-size $beta*n$ --benchmark $PATH_TO_BENCHMARK$
```

+ --dataset: the path to dataset file
+ --leaf-size: a positive integer, the max size of a leaf node in DE-Tree
+ --dataset-size: n, a positive integer, the cardinality of dataset
+ --queries: the path to query file 
+ --queries-size: a positive integer, the number of queries in the query file
+ --sample-size: a positive integer, the number of sampled data in the encoding phase, usually set to 0.1n
+ --sample-type: the type of sampling, 1 is first-n-sampling, 2 is uniform sampling, 3 is random sampling
+ --k-size: a positive integer, the number of returned points for c-k-ANN queries
+ --data-dimensionality: a positive integer, the dimensionality of dataset
+ --l-size: L, a positive integer, the number of indexes
+ --search-radius: r, a float number, the initial search radius
+ --max-candidate-size: a positive integer, the maximum number of returned points, i.e., beta*n
+ --benchmark: the path to benchmark file

The specific commands and parameters for each dataset of DET-LSH are written in file `run_DETLSH.sh`.

## Dataset, Query, and Benchmark

#### Dataset

We use eight real-world datasets for ANN search: Msong, Deep1M, Sift10M, TinyImages80M, Sift100M, Yandex Deep500M, Microsoft SPACEV500M, and Microsoft Turing-ANNS500M. The key statistics of the datasets are summarized as follows.

|          Dataset          | Cardinality | Dimensions | Type  |
| :-----------------------: | :---------: | :--------: | :---: |
|           Msong           |   994,185   |    420     | Audio |
|          Deep1M           |  1,000,000  |    256     | Image |
|          Sift10M          | 10,000,000  |    128     | Image |
|       TinyImages80M       | 79,302,017  |    384     | Image |
|         Sift100M          | 100,000,000 |    128     | Image |
|      Yandex Deep500M      | 500,000,000 |     96     | Image |
|   Microsoft SPACEV500M    | 500,000,000 |    100     | Text  |
| Microsoft Turing-ANNS500M | 500,000,000 |    100     | Text  |

#### Query

We randomly select 100 data points as queries and remove them from the original datasets.

#### Format of Dataset and Query

Our input dataset file and query file is in binary format without any indice, i.e., the binary file is organized as the following format:

```
{The binary vectors of data points, arranged in turn}
{The binary vectors of query points, arranged in turn}
```

#### Benchmark

The format of benchmark file is:

```
{The Euclidean distances of exact k-NN points to each query point, arranged in turn}
```

