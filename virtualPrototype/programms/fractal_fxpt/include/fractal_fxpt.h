#ifndef FRACTAL_FXPT_H
#define FRACTAL_FXPT_H

#include <stdint.h>

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

// Q7.25 fixed point type
typedef int32_t fxpt_7_25;

inline fxpt_7_25 float_to_fix(float f)
{
    return (fxpt_7_25)(f * (1 << 25));
}

inline float fix_to_float(fxpt_7_25 f)
{
    return (float)(f) / (1 << 25);
}

inline fxpt_7_25 mul(fxpt_7_25 a, fxpt_7_25 b)
{
    int64_t product = (int64_t)a * (int64_t)b; // Q14.50
    return (fxpt_7_25)((product >> 25) & 0xffffffff);
}

inline fxpt_7_25 div(fxpt_7_25 a, fxpt_7_25 b)
{
    // 101.00011
    // 010.00000
    // 10.100011
    int64_t quotient = a / b;
}

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(fxpt_7_25 cx, fxpt_7_25 cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(fxpt_7_25 cx, fxpt_7_25 cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  fxpt_7_25 cx_0, fxpt_7_25 cy_0, fxpt_7_25 delta, uint16_t n_max);

#endif // FRACTAL_FXPT_H
