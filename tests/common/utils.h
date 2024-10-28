#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

typedef uint16_t rgb565;

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

#endif