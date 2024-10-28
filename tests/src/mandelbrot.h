#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdint.h>

typedef uint16_t rgb565;

uint16_t calc_mandelbrot_point(float cx, float cy, uint16_t n_max);
rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

#endif