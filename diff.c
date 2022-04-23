#include <stdbool.h>
#include "diff.h"
#include "ctype.h"
#include "stdio.h"
#include "smart_buffer.h"

// TODO delete smart_buffer
int is_printable(char c) {
    return isprint(c) || c == '\0' || c == '\n';
}

void report_text_diff(char first_file_buffer[BUFFER_SIZE], char second_file_buffer[BUFFER_SIZE], size_t n){
    printf("%s", first_file_buffer);
    for (int i = 0; i < n; ++i) {
        first_file_buffer[i] != second_file_buffer[i] ? printf("+") : printf(" ");
    }
    printf("%s", second_file_buffer);
}

bool check_buf_is_printable(char buf[BUFFER_SIZE], size_t n){
    for (int i = 0; i < n; ++i) {
        if(!is_printable(buf[i])) return false;
    }
    return true;
}


bool check_diff(const char *first_file_buffer, const char *second_file_buffer, bool diff_found, size_t n) {
    for (int i = 0; i < n; ++i) {
        if (first_file_buffer[i] == second_file_buffer[i] && diff_found) break;
        if (first_file_buffer[i] != second_file_buffer[i]) {
            diff_found = true;
        }
    }
    return diff_found;
}

void compare_files(struct T_file first, struct T_file second) {
    char first_file_buffer[BUFFER_SIZE];
    char second_file_buffer[BUFFER_SIZE];

    bool diff_found = false;
    bool is_buff_printable;
    size_t lines_count = 1;
    size_t bytes_count = 0;




    while (1) {
        size_t n = fread(first_file_buffer, 1, sizeof(first_file_buffer), first.file);
        if (n == 0) break;
        n = fread(second_file_buffer, 1, sizeof(second_file_buffer), second.file);
        if (n == 0) break;
        is_buff_printable = check_buf_is_printable(first_file_buffer, n) && check_buf_is_printable(second_file_buffer, n);
        diff_found = check_diff(first_file_buffer, second_file_buffer, diff_found, n);
        if(diff_found && is_buff_printable){
            report_text_diff(first_file_buffer, second_file_buffer, n);
            break;
        }

    }


}
