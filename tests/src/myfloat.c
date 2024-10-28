#include "utils.h"

#include <stb_image_write.h>

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define EXPONENT_EXCESS 122
#define MANTISSA_BITS 24

typedef struct
{
    uint32_t sign : 1;
    uint32_t exponent : 32 - MANTISSA_BITS - 1;
    uint32_t mantissa : MANTISSA_BITS;
} my_float;

float my_float_to_float(my_float f)
{
    int32_t exponent = f.exponent - EXPONENT_EXCESS;
    uint32_t mantissa = f.mantissa | (1 << MANTISSA_BITS);
    float exp_float = (float)exponent;
    float mant_float = (float)mantissa / (float)(1 << MANTISSA_BITS);
    float result = powf(2, exp_float) * mant_float;
    if (f.sign)
        result *= -1.0f;
    return result;
}

my_float add(my_float a, my_float b)
{
    int32_t exponent_a = a.exponent - EXPONENT_EXCESS;
    int32_t exponent_b = b.exponent - EXPONENT_EXCESS;
    // We need to add the implicit leading 1
    uint32_t mantissa_a = a.mantissa | (1 << MANTISSA_BITS);
    uint32_t mantissa_b = b.mantissa | (1 << MANTISSA_BITS);

    // Align the exponents
    if (exponent_a > exponent_b)
    {
        mantissa_b >>= exponent_a - exponent_b;
        exponent_b = exponent_a;
    }
    else if (exponent_b > exponent_a)
    {
        mantissa_a >>= exponent_b - exponent_a;
        exponent_a = exponent_b;
    }

    // Add or subtract mantissas
    uint32_t result_sign = a.sign;
    uint32_t result_mantissa;
    if (a.sign == b.sign)
    {
        result_mantissa = mantissa_a + mantissa_b;
    }
    else if (mantissa_a > mantissa_b)
    {
        result_mantissa = mantissa_a - mantissa_b;
    }
    else
    {
        result_mantissa = mantissa_b - mantissa_a;
        result_sign = b.sign;
    }

    // Handle overflow
    uint32_t result_exponent = exponent_a + EXPONENT_EXCESS;
    if (result_mantissa & (1 << (MANTISSA_BITS + 1)))
    {
        result_mantissa >>= 1;
        result_exponent += 1;
    }

    // Normalize the mantissa by shifting the leading 1 into the correct position
    while ((result_mantissa & (1 << MANTISSA_BITS)) == 0 && result_exponent > 0)
    {
        result_mantissa <<= 1;
        result_exponent -= 1;
    }
    // Remove the implicit leading 1
    result_mantissa &= (1 << MANTISSA_BITS) - 1;

    my_float result = {.sign = result_sign, .exponent = result_exponent, .mantissa = result_mantissa};
    return result;
}

my_float sub(my_float a, my_float b)
{
    b.sign = !b.sign;
    return add(a, b);
}

my_float mul(my_float a, my_float b)
{
    uint32_t result_sign = a.sign ^ b.sign;

    // Add exponents
    int32_t result_exponent = a.exponent + b.exponent - EXPONENT_EXCESS;

    // Multiply mantissas
    uint64_t mantissa_a = (uint64_t)(a.mantissa | (1 << MANTISSA_BITS));
    uint64_t mantissa_b = (uint64_t)(b.mantissa | (1 << MANTISSA_BITS));
    uint64_t mantissa_product = mantissa_a * mantissa_b;

    printf("%llx\n", mantissa_product);
    // Normalize the product
    if (mantissa_product & (1ull << (2 * MANTISSA_BITS + 1)))
    {
        // The product overflowed to an extra bit
        mantissa_product >>= MANTISSA_BITS + 2;
        result_exponent += 1;
    }
    else
    {
        mantissa_product >>= MANTISSA_BITS + 1;
    }
    printf("%llx\n", mantissa_product);

    // Remove implicit leading 1
    uint32_t result_mantissa = (uint32_t)mantissa_product & ((1 << MANTISSA_BITS) - 1);

    my_float result = {.sign = result_sign, .exponent = (uint32_t)result_exponent, .mantissa = result_mantissa};
    return result;
}

uint16_t calc_mandelbrot_point(float cx, float cy, uint16_t n_max)
{
    float x = cx;
    float y = cy;
    uint16_t n = 0;
    float xx, yy, two_xy;
    do
    {
        xx = x * x;
        yy = y * y;
        two_xy = 2.0f * x * y;

        x = xx - yy + cx;
        y = two_xy + cy;
        ++n;
    } while (((xx + yy) < 4.0f) && (n < n_max));
    return n;
}

int main()
{
    my_float a = {0, 123, 0x600000}; // 2.75
    my_float b = {1, 121, 0xc00000}; // -0.875
    my_float c = add(a, b);
    my_float d = sub(a, b);
    my_float e = mul(a, b);
    printf("%f + %f = %f\n", my_float_to_float(a), my_float_to_float(b), my_float_to_float(c));
    printf("%f - %f = %f\n", my_float_to_float(a), my_float_to_float(b), my_float_to_float(d));
    printf("%f * %f = %f\n", my_float_to_float(a), my_float_to_float(b), my_float_to_float(e));
    return 0;

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
