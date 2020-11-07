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

//int main(int argc, char** argv) {
//
//    char* input = "rgb24pal.bmp";//argv[1];
//    char* output = "output.bmp";
//    FILE* bmp_input_file = fopen(input, "rb");
//    FILE* bmp_output_file = fopen(output, "wb");
//
//    bitmap_file_header file_header;
//    bitmap_info_header info_header;
//
//    fread(&file_header, sizeof(bitmap_file_header), 1, bmp_input_file);
//    fread(&info_header, sizeof(bitmap_info_header), 1, bmp_input_file);
//
//    fwrite(&file_header, sizeof(bitmap_file_header), 1, bmp_output_file);
//    fwrite(&info_header, sizeof(bitmap_info_header), 1, bmp_output_file);
//
//    if (info_header.biBitCount = 24 ) {
//
//        for (int i = 0; i < info_header.biHeight; ++i) {
//            for (int j = 0; j < info_header.biWidth; ++j) {
//                for (int k = 0; k < 3; ++k) {
//                    unsigned char byte = fgetc(bmp_input_file);
//                    fputc(~byte, bmp_output_file);
//                }
//            }
//        }
//    }
//
//    if (info_header.biBitCount = 8 ) {
//
//    }
//
//
//
//
//
//    fclose(bmp_input_file);
//    fclose(bmp_output_file);
//
//
//
//
//    UCHAR	r, g, b, r2, g2, b2;
//    UINT	width, height, width2, height2;
//    UINT	x, y;
//    BMP*	bmp;
//
//    bmp = BMP_ReadFile(input);
//
//    width = BMP_GetWidth( bmp );
//    height = BMP_GetHeight( bmp );
//
// Iterate through all the image's pixels
//
//    for ( x = 0 ; x < width ; ++x )
//    {
//        for ( y = 0 ; y < height ; ++y )
//        {
// Get pixel's RGB values
//
//            BMP_GetPixelRGB( bmp, x, y, &r, &g, &b );
//
// Invert RGB values
//
//            BMP_SetPixelRGB( bmp, x, y, 255 - r, 255 - g, 255 - b );
//        }
//    }
//
//    BMP_WriteFile(bmp, "output2.bmp");
//
//    BMP_Free(bmp);
//
//    BMP* left = BMP_ReadFile("output.bmp");
//    BMP* right = BMP_ReadFile("output2.bmp");
//
//    width =  BMP_GetWidth(left);
//    height = BMP_GetHeight(left);
//
//    width2 = BMP_GetWidth(right);
//    height2 = BMP_GetHeight(right);
//
//    if (width != width2 || height != height2)
//        return 1;
//
//    for (int i = 0; i < width; ++i) {
//        for (int j = 0; j < height; ++j) {
//            BMP_GetPixelRGB(left, i, j, &r, &g, &b);
//            BMP_GetPixelRGB(right, i, j, &r2, &g2, &b2);
//
//            if (r != r2 || g != g2 || b != b2) {
//                return 2;
//            }
//        }
//    }
//
//    return 0;
//}
