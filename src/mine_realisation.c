#include "mine_realisation.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEVIATION_COUNT 100

int read_file(char* filename, bitmap* bmp){
    FILE* bmp_input_file = fopen(filename, "rb");

    if (bmp_input_file == NULL) {
        fprintf(stderr, "File not found");
        return -1;
    }

    if (fread(&(bmp->file_header), 1, sizeof(bitmap_file_header), bmp_input_file) != sizeof(bitmap_file_header)) {
        fprintf(stderr, "Bad reading FileHeader");
        fclose(bmp_input_file);
        return -2;
    }

    if (bmp->file_header.bfType != 0x4D42) {
        fprintf(stderr, "This is not BMP file");
        fclose(bmp_input_file);
        return -2;
    }

    if ( fread(&(bmp->info_header), 1, sizeof(bitmap_info_header), bmp_input_file) != sizeof(bitmap_info_header)) {
        fprintf(stderr, "Bad reading InfoHeader");
        fclose(bmp_input_file);
        return -2;
    }

    if (bmp->info_header.biSize != 40) {
        fprintf(stderr, "Unsupported version bmp file");
        fclose(bmp_input_file);
        return -2;
    }

    if (bmp->info_header.biBitCount != 8 && bmp->info_header.biBitCount != 24) {
        fprintf(stderr, "Unsupported bit count");
        fclose(bmp_input_file);
        return -2;
    }

    if (bmp->info_header.biClrUsed == 0)
        bmp->palette = NULL;
    else {
        unsigned long palette_size = sizeof(rgbquad) * bmp->info_header.biClrUsed;

        bmp->palette = malloc(palette_size);
        if (fread(bmp->palette, 1, palette_size, bmp_input_file) != palette_size) {
            fprintf(stderr, "Bad reading palette");
            fclose(bmp_input_file);
            free(bmp->palette);
            return -2;
        }
    }

    bmp->data = malloc(bmp->info_header.biSizeImage);

    if (fread(bmp->data, sizeof(BYTE), bmp->info_header.biSizeImage, bmp_input_file) != bmp->info_header.biSizeImage) {
        fprintf(stderr, "Bad reading data");
        fclose(bmp_input_file);
        free(bmp->palette);
        free(bmp->data);
        return -2;
    }

    fclose(bmp_input_file);
    return 0;
}

int invert_colors(char* output_file, bitmap* bmp) {
    FILE* output_bmp_file = fopen(output_file, "wb");

    if (fwrite(&(bmp->file_header), 1, sizeof(bitmap_file_header), output_bmp_file) != sizeof(bitmap_file_header)) {
        fprintf(stderr, "Bad writing file header to output");
        fclose(output_bmp_file);
        return -2;
    }

    if (fwrite(&(bmp->info_header), 1, sizeof(bitmap_info_header), output_bmp_file) != sizeof(bitmap_info_header)) {
        fprintf(stderr, "Bad writing info header to output");
        fclose(output_bmp_file);
        return -2;
    }

    if (bmp->info_header.biBitCount == 24) {
        if (bmp->palette != NULL) {
            unsigned long palette_size = sizeof(rgbquad) * bmp->info_header.biClrUsed;

            if (fwrite(bmp->palette, 1, palette_size, output_bmp_file) != palette_size) {
                fprintf(stderr, "Bad writing palette to output");
                fclose(output_bmp_file);
                return -2;
            }
        }

        int count = 0;
        int extra_bytes = (4 - bmp->info_header.biWidth % 4) % 4;

        for (int i = 0; i < bmp->info_header.biHeight; ++i) {
            for (int j = 0; j < bmp->info_header.biWidth; ++j) {
                for (int k = 0; k < 3; ++k) {
                    BYTE byte = bmp->data[count++];
                    fputc(~byte, output_bmp_file);
                }
            }

            for (int j = 0; j < extra_bytes; ++j) {
                BYTE byte = bmp->data[count++];
                fputc(byte, output_bmp_file);
            }
        }
    } else if (bmp->info_header.biBitCount == 8) {
        if (bmp->palette == NULL) {
            fprintf(stderr, "Palette cannot be NULL in 8bit image");
            fclose(output_bmp_file);
            return -2;
        }

        for (int i = 0; i < bmp->info_header.biClrUsed; ++i) {
            rgbquad pixel = bmp->palette[i];
            fputc(~pixel.blue, output_bmp_file);
            fputc(~pixel.green, output_bmp_file);
            fputc(~pixel.red, output_bmp_file);
            fputc(pixel.reserved, output_bmp_file);
        }

        if (fwrite(bmp->data, 1, bmp->info_header.biSizeImage, output_bmp_file) != bmp->info_header.biSizeImage) {
            fprintf(stderr, "Bad writing data to output");
            fclose(output_bmp_file);
        }
    }

    fclose(output_bmp_file);
    return 0;
}

int compare_pixel(rgbquad p1, rgbquad p2) {
    return (p1.blue - p2.blue || p1.green - p2.green || p1.red - p2.red);
}

int compare_BMP(bitmap *image1, bitmap *image2) {
    if (image1->info_header.biBitCount != image2->info_header.biBitCount) {
        printf("Images have different bits per pixel\n");
        return -1;
    }
    LONG width1 = image1->info_header.biWidth;
    LONG height1 = image1->info_header.biHeight;
    LONG width2 = image2->info_header.biWidth;
    LONG height2 = image2->info_header.biHeight;
    if (width1 != width2) {
        fprintf(stderr, "Images have different widths\n");
        return -1;
    }
    if (abs(height1) != abs(height2)) {
        fprintf(stderr, "Images have different widths\n");
        return -1;
    }
    int deviation_count = 0;
    int abs_height = abs(height1);
    BYTE bytes_per_pixel = image1->info_header.biBitCount / 8;
    WORD bytes_per_row = image1->info_header.biSizeImage / abs(image1->info_header.biHeight);
    if (image2->info_header.biHeight == 24) {
        for (int i = 0; i < abs_height && deviation_count < MAX_DEVIATION_COUNT; ++i) {
            for (int j = 0; j < width1 && deviation_count < MAX_DEVIATION_COUNT; ++j) {
                for(int k = 0; k < 3; ++k) {
                    BYTE b1 = image1->data[i * width1 + j * bytes_per_pixel + k];
                    BYTE b2 = image2->data[i * width1 + j * bytes_per_pixel + k];
                    if (b1 != b2) {
                        printf("%d %d", i, j);
                        deviation_count++;
                        break;
                    }
                }
                printf("\n");
            }
        }
    } else {
        for (int i = 0; i < abs_height && deviation_count < MAX_DEVIATION_COUNT; ++i) {
            for (int j = 0; j < width1 && deviation_count < MAX_DEVIATION_COUNT; ++j) {
                for(int k = 0; k < 3; ++k) {
                    BYTE b1 = image1->data[i * width1 + j * bytes_per_pixel + k];
                    BYTE b2 = image2->data[i * width1 + j * bytes_per_pixel + k];
                    if (compare_pixel(image1->palette[b1], image2->palette[b2]) != 0) {
                        printf("%d %d", i, j);
                        deviation_count++;
                        printf("\n");
                        break;
                    }
                }
            }
        }
    }
    return 0;
}