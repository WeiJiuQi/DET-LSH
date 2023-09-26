#ifndef isaxlib_isax_node_record_h
#define isaxlib_isax_node_record_h
#include "../../../config.h"
#include "../../../globals.h"

typedef struct {
    sax_type *sax;
    data_type *ts;
    file_position_type *position;
    enum insertion_mode insertion_mode;
    void *destination;
} isax_node_record;


isax_node_record * isax_node_record_init(int sax_size, int ts_size, 
                                         enum insertion_mode);
void isax_node_record_destroy(isax_node_record *node);
#endif
