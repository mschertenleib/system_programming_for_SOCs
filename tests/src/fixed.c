#include "utils.h"

#include <stb_image_write.h>

#include <stdio.h>
#include <stdint.h>

typedef int32_t fxpt_7_25;
#define FRACTIONAL_BITS 25

#define float_to_fix(flpt) (fxpt_7_25)((flpt) * (float)(1 << FRACTIONAL_BITS))
#define fix_to_float(fxpt) ((float)(fxpt) / (float)(1 << FRACTIONAL_BITS))

inline fxpt_7_25 mul(fxpt_7_25 a, fxpt_7_25 b)
{
    int64_t product = (int64_t)a * (int64_t)b;
    return (fxpt_7_25)(product >> FRACTIONAL_BITS);
}

uint16_t calc_mandelbrot_point(fxpt_7_25 cx, fxpt_7_25 cy, uint16_t n_max)
{
    fxpt_7_25 x = cx;
    fxpt_7_25 y = cy;
    uint16_t n = 0;
    fxpt_7_25 xx, yy, two_xy;
    const fxpt_7_25 four = float_to_fix(4.0f);
    do
    {
        xx = mul(x, x);
        yy = mul(y, y);
        two_xy = mul(x, y) << 1;
        x = xx - yy + cx;
        y = two_xy + cy;
        ++n;
    } while (((xx + yy) < four) && (n < n_max));
    return n;
}

int main()
{
    const int width = 512;
    const int height = 512;
    const float fractal_width = 3.0f;
    const fxpt_7_25 cx_0 = float_to_fix(-2.0f);
    const fxpt_7_25 cy_0 = float_to_fix(-1.5f);
    const uint16_t n_max = 64;

    uint8_t *image = malloc(height * width * 3);
    const fxpt_7_25 delta = float_to_fix((float)fractal_width / (float)width);

    uint8_t *pixel_channel = image;
    fxpt_7_25 cy = cy_0;
    for (int k = 0; k < height; ++k)
    {
        fxpt_7_25 cx = cx_0;
        for (int i = 0; i < width; ++i)
        {
            uint16_t n_iter = calc_mandelbrot_point(cx, cy, n_max);
            rgb565 colour = iter_to_colour(n_iter, n_max);
            float r = (float)((colour & 0xf800) >> 11) / 31.0f;
            float g = (float)((colour & 0x07e0) >> 5) / 63.0f;
            float b = (float)(colour & 0x001f) / 31.0f;
            *(pixel_channel++) = (uint8_t)(r * 255.0f);
            *(pixel_channel++) = (uint8_t)(g * 255.0f);
            *(pixel_channel++) = (uint8_t)(b * 255.0f);
            cx += delta;
        }
        cy += delta;
    }

    if (!stbi_write_png("mandelbrot.png", width, height, 3, image, width * 3))
    {
        fprintf(stderr, "Failed to write PNG image");
    }
    free(image);

    return 0;
}
