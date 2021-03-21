#include "other_realisation.h"
#include "qdbmp.h"


int invert_colors_other(char* input, char* output) {
    BMP* input_bmp = BMP_ReadFile(input);
    BMP_CHECK_ERROR( stderr, -3 );


    UINT width = BMP_GetWidth(input_bmp);
    UINT height = BMP_GetHeight(input_bmp);

    UCHAR r, g, b;

    if (BMP_GetDepth(input_bmp) == 24) {
        for (int x = 0 ; x < width ; ++x) {
            for (int y = 0 ; y < height ; ++y) {
                /* Get pixel's RGB values */
                BMP_GetPixelRGB( input_bmp, x, y, &r, &g, &b );

                /* Invert RGB values */
                BMP_SetPixelRGB( input_bmp, x, y, 255 - r, 255 - g, 255 - b );
            }
        }
    } else if (BMP_GetDepth(input_bmp) == 8) {
        for (int i = 0; i < 256; ++i) {
            BMP_GetPaletteColor(input_bmp, i, &r, &g, &b);

            BMP_SetPaletteColor(input_bmp, i, 255 - r, 255 - g, 255 - b);
        }
    }

    BMP_WriteFile(input_bmp, output);
    BMP_CHECK_ERROR(stderr, -3);

    BMP_Free(input_bmp);
    return 0;
}