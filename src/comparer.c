#include <stdlib.h>
#include <stdio.h>
#include "mine_realisation.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "No! No! No! Wrong number of parameters!\n");
        return -1;
    }
    char *first_file = argv[1];
    char *second_file = argv[2];
    int return_code;
    bitmap image1;
    bitmap image2;
    return_code = read_file(first_file, &image1);
    if (return_code != 0) {
        fprintf(stderr, "Cannot read image 1\n");
    }
    return_code = read_file(second_file, &image2);
    if (return_code != 0) {
        fprintf(stderr, "Cannot read image 2\n");
    }
    int res = compare_BMP(&image1, &image2);
    if (res != 0) {
        return res;
    }
    return 0;
}