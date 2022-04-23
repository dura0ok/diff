#pragma once

#include <stdlib.h>
#include "t_file.h"
#include "smart_buffer.h"
#define BUFFER_SIZE 16

struct T_diff_s {
    struct SmartBuf *first, *second;
    size_t size;
    off_t offset;
    int printable;
};


void compare_files(struct T_file first, struct T_file second);
