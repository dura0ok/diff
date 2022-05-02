#include <stdbool.h>
#include "diff.h"
#include "ctype.h"
#include "stdio.h"
#include "smart_buffer.h"
#include "string.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


void report_text_diff(char *str1, char *str2) {
    printf("%s\n", str1);
    int i = 0;
    while (str1[i] && str2[i]) {
        if (str1[i] == str2[i])
            putchar(' ');
        else
            putchar('+');
        i++;
    }
    if (str1[i])
        while (str1[i]) {
            putchar('^');
            i++;
        }
    else
        while (str2[i]) {
            putchar('v');
            i++;
        }
    putchar('\n');
    printf("%s\n", str2);
}

int is_printable(char c) {
    return isprint(c) || c == '\0' || c == '\n';
}

bool check_buf_is_printable(char buf[BUFFER_SIZE], size_t n){
    for (int i = 0; i < n; ++i) {
        if(!is_printable(buf[i])) return false;
    }
    return true;
}

// return -1 if diff not found in char array
int get_diff_offset(const char *first_file_buffer, const char *second_file_buffer, size_t n) {
    int offset = 0;
    for (int i = 0; i < n; i++) {
        offset++;
        if (first_file_buffer[i] != second_file_buffer[i]) {
            return offset;
        }
    }
    return -1;
}


void print_buf_by_hex(const char *buf, size_t n) {
    size_t byte_counter = 0;
    for (int i = 0; i < n; ++i) {
        if(i == 15) printf(" | ");
        printf("%02x ", (unsigned char)buf[i]);
        byte_counter++;
    }
    printf("\n");
}

void report_binary_diff(const char first_file_buffer[BUFFER_SIZE], const char second_file_buffer[BUFFER_SIZE], size_t n){
    print_buf_by_hex(first_file_buffer, n);
    for (int i = 0; i < n; i++) {
        if(i == 15) printf("   ");
        first_file_buffer[i] != second_file_buffer[i] ? printf("++ ") : printf("   ");
    }
    printf("\n");
    print_buf_by_hex(second_file_buffer, n);
}


void clear_line_buffers(struct SmartBuf *first_sub_buf, struct SmartBuf *second_sub_buf) {
    first_sub_buf->length = 0;
    memset(&first_sub_buf->buf[0], 0, sizeof(first_sub_buf->buf[0]));
    second_sub_buf->length = 0;
    memset(&second_sub_buf->buf[0], 0, sizeof(second_sub_buf->buf[0]));
}

void handle_offset(struct SmartBuf *first_sub_buf, struct SmartBuf *second_sub_buf, size_t lines_count, size_t bytes_count,
              int offset) {
    if(offset <= 0) return;
    printf("\nDiscrepancy at byte %lu, at line %lu\n", bytes_count + offset, lines_count);
    if(check_buf_is_printable(first_sub_buf->buf, first_sub_buf->length) && check_buf_is_printable(second_sub_buf->buf, second_sub_buf->length)){
        report_text_diff(first_sub_buf->buf, second_sub_buf->buf);
    }else{
        report_binary_diff(first_sub_buf->buf, second_sub_buf->buf, first_sub_buf->length);
    }
}

void compare_files(struct T_file first, struct T_file second) {
    char first_file_buffer[BUFFER_SIZE] = {'\0'};
    char second_file_buffer[BUFFER_SIZE] = {'\0'};
    struct SmartBuf* first_sub_buf = create_smart_buffer();
    struct SmartBuf* second_sub_buf = create_smart_buffer();

    size_t lines_count = 1;
    size_t bytes_count = 0;


    int offset;

    while (1) {
        size_t first_file_bytes_read  = fread(first_file_buffer, 1, sizeof(first_file_buffer), first.file);
        size_t second_file_bytes_read = fread(second_file_buffer, 1, sizeof(second_file_buffer), second.file);
        if (first_file_bytes_read == 0 && second_file_bytes_read == 0) break;
        size_t n = max(first_file_bytes_read, second_file_bytes_read);

        if(get_diff_offset(first_file_buffer, second_file_buffer, n) > 0){
            for (int i = 0; i < n; ++i) {
                bytes_count += 1;
                if(first_file_buffer[i] == '\n' && second_file_buffer[i] == '\n'){
                    lines_count++;
                    offset = get_diff_offset(first_sub_buf->buf, second_sub_buf->buf, first_sub_buf->length);
                    handle_offset(first_sub_buf, second_sub_buf, lines_count, bytes_count, offset);

                    clear_line_buffers(first_sub_buf, second_sub_buf);
                    continue;
                }
                push_char_to_buffer(first_sub_buf, first_file_buffer[i]);
                push_char_to_buffer(second_sub_buf, second_file_buffer[i]);
            }
            offset = get_diff_offset(first_sub_buf->buf, second_sub_buf->buf, first_sub_buf->length);
            handle_offset(first_sub_buf, second_sub_buf, lines_count, bytes_count, offset);
            break;
        }
    }

    free_smart_buffer(first_sub_buf);
    free_smart_buffer(second_sub_buf);

}
