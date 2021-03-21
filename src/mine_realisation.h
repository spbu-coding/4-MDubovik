#ifndef TESTS_MINE_REALISATION_H
#define TESTS_MINE_REALISATION_H
#include <inttypes.h>

typedef unsigned char BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

#pragma pack(push, 1)
typedef struct bitmap_file_header_ {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} bitmap_file_header;

typedef struct bitmap_info_header_ {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} bitmap_info_header;

typedef struct rgbquad_ {
    BYTE blue;
    BYTE green;
    BYTE red;
    BYTE reserved;
} rgbquad;
#pragma pack(pop)

typedef rgbquad* bitmap_palette;

typedef BYTE* bitmap_data;

typedef struct bitmap_ {
    bitmap_file_header file_header;
    bitmap_info_header info_header;
    bitmap_palette palette;
    bitmap_data data;
} bitmap;

int read_file(char* filename, bitmap* bmp);
int invert_colors(char* output_file, bitmap* bmp);
int compare_BMP(bitmap *image1, bitmap *image2);

#endif //TESTS_MINE_REALISATION_H