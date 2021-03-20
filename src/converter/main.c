#include "mine_realisation.h"
#include "other_realisation.h"
#include "other/qdbmp.h"
#include <stdio.h>
#include <string.h>

typedef enum realisation_ {
    Mine,
    Other
} realisation;

int analyse_parameters(int argc, char** argv, realisation* realisation, char** input, char** output) {
    if (argc != 4) {
        fprintf(stderr, "The number of arguments must be 3");
        return -1;
    }

    if (strcmp(argv[1], "--mine") == 0)
        *realisation = Mine;
    else if (strcmp(argv[1], "--theirs") == 0)
        *realisation = Other;
    else {
        fprintf(stderr, "The first argument must be --mine or --theirs");
        return -1;
    }

    *input = argv[2];
    *output = argv[3];
    return 0;
}

int main(int argc, char** argv) {
    realisation realisation;
    char* input;
    char* output;
    int return_code;

    analyse_parameters(argc, argv, &realisation, &input, &output);

    if (realisation == Mine) {
        bitmap bmp;
        return_code = read_file(input, &bmp);

        if (return_code != 0)
            return return_code;

        return_code = invert_colors(output, &bmp);

        if (return_code != 0)
            return return_code;
    } else if (realisation == Other) {
        return_code = invert_colors_other(input, output);
        if (return_code != 0)
            return return_code;
    }

    return 0;
}
