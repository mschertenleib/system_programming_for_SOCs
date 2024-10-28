#include "mandelbrot.h"

#include <stb_image_write.h>

#include <stdio.h>
#include <stdint.h>

int main()
{
    const int width = 512;
    const int height = 512;
    const float fractal_width = 3.0f;
    const float cx_0 = -2.0f;
    const float cy_0 = -1.5f;
    const uint16_t n_max = 64;

    uint8_t *image = malloc(height * width * 3);
    const float delta = (float)fractal_width / (float)width;

    uint8_t *pixel_channel = image;
    float cy = cy_0;
    for (int k = 0; k < height; ++k)
    {
        float cx = cx_0;
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
