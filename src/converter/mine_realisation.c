#include "mine_realisation.h"
#include <stdio.h>
#include <stdlib.h>

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

