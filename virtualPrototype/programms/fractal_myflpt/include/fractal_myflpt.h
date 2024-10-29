#ifndef FRACTAL_MYFLPT_H
#define FRACTAL_MYFLPT_H

#include <stdint.h>

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

#define EXPONENT_EXCESS 122
#define MANTISSA_BITS 24

typedef struct
{
    uint32_t sign : 1;
    uint32_t exponent : 32 - MANTISSA_BITS - 1;
    uint32_t mantissa : MANTISSA_BITS;
} my_float;

inline my_float add(my_float a, my_float b)
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

inline my_float sub(my_float a, my_float b)
{
    b.sign = !b.sign;
    return add(a, b);
}

inline my_float mul(my_float a, my_float b)
{
    uint32_t result_sign = a.sign ^ b.sign;

    // Add exponents
    int32_t result_exponent = a.exponent + b.exponent - EXPONENT_EXCESS;
    // Underflow (result is too small to be representable)
    if (result_exponent < 0)
    {
        return (my_float){0};
    }

    // Multiply mantissas
    uint64_t mantissa_a = (uint64_t)(a.mantissa | (1 << MANTISSA_BITS));
    uint64_t mantissa_b = (uint64_t)(b.mantissa | (1 << MANTISSA_BITS));
    // 2 integral bits and 2 * MANTISSA_BITS fractional bits
    uint64_t mantissa_product = mantissa_a * mantissa_b;

    // If the second integral bit is set
    if (mantissa_product & ((uint64_t)1 << (2 * MANTISSA_BITS + 1)))
    {
        mantissa_product >>= 1;
        result_exponent += 1;
    }
    // Normalize the product to a MANTISSA_BITS + 1 mantissa, truncating trailing bits
    mantissa_product >>= MANTISSA_BITS;

    // Remove implicit leading 1
    uint32_t result_mantissa = (uint32_t)mantissa_product & ((1 << MANTISSA_BITS) - 1);

    my_float result = {.sign = result_sign,
                       .exponent = (uint32_t)result_exponent,
                       .mantissa = result_mantissa};
    return result;
}

inline int less_than(my_float a, my_float b)
{
    // Signs are different
    if (a.sign ^ b.sign)
    {
        return a.sign > b.sign;
    }

    // Signs are the same
    int comp;
    if (a.exponent != b.exponent)
    {
        comp = a.exponent < b.exponent;
    }
    else
    {
        comp = a.mantissa < b.mantissa;
    }

    // If both are negative, reverse the comparison
    return a.sign ? !comp : comp;
}

my_float float_to_my_float(float f);

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(my_float cx, my_float cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(my_float cx, my_float cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  my_float cx_0, my_float cy_0, my_float delta, uint16_t n_max);

#endif // FRACTAL_MYFLPT_H
