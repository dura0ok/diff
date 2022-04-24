#include <stdio.h>
#include "t_file.h"
#include "diff.h"

void close_data(struct T_file first, struct T_file second){
    close_file(first);
    close_file(second);
}

int main() {
    struct T_file first = open_file("kek1");
    struct T_file second = open_file("kek2");
    long first_size = get_file_size(first);
    long second_size = get_file_size(second);

    printf("%s %s\n", first.file_path, second.file_path);
    if(first_size != second_size){
        printf("Size mismatch: file1 is %ld bytes, file 2 is %ld bytes\n", first_size, second_size);
    }

    compare_files(first, second);

    close_data(first, second);
    return 0;
}
