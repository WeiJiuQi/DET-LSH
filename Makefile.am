AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS =  ${ACLOCAL_FLAGS}

lib_LIBRARIES	= lib/libutils.a
lib_libutils_a_SOURCES = src/utils/isax/isax_first_buffer_layer.c src/utils/isax/isax_index.c src/utils/isax/isax_node.c src/utils/isax/isax_node_buffer.c src/utils/isax/isax_node_record.c src/utils/isax/isax_node_split.c src/utils/isax/pqueue.c src/utils/sax/sax.c src/utils/sax/ts.c src/utils/query_support.c src/utils/indexing_support.c src/utils/encoding_support.c
lib_libutils_a_CFLAGS =-I/opt/local/include -Iinclude/ -march=native -mavx -mavx2 -msse3 -fopenmp

bin_PROGRAMS = bin/DETLSH
bin_DETLSH_SOURCES = src/main/DETLSH.cpp
bin_DETLSH_CXXFLAGS = -I/opt/local/include -Iinclude/ -I/usr/local/include/ -march=native -fopenmp
bin_DETLSH_LDADD = -lm -lreadline -lutils -lpthread
bin_DETLSH_LDFLAGS = -L/opt/local/lib -Llib/  -march=native -mavx -mavx2 -msse3

