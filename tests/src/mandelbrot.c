#include "mandelbrot.h"

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

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max)
{
    if (iter == n_max)
    {
        return 0x0000;
    }
    return 0xffff;
}

rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max)
{
    if (iter == n_max)
    {
        return 0x0000;
    }
    uint16_t brightness = iter & 0xf;
    return ((brightness << 12) | ((brightness << 7) | brightness << 1));
}

int ilog2(unsigned x)
{
    if (x == 0)
        return -1;
    int n = 1;
    if ((x >> 16) == 0)
    {
        n += 16;
        x <<= 16;
    }
    if ((x >> 24) == 0)
    {
        n += 8;
        x <<= 8;
    }
    if ((x >> 28) == 0)
    {
        n += 4;
        x <<= 4;
    }
    if ((x >> 30) == 0)
    {
        n += 2;
        x <<= 2;
    }
    n -= (int)(x >> 31);
    return 31 - n;
}

rgb565 iter_to_colour(uint16_t iter, uint16_t n_max)
{
    if (iter == n_max)
    {
        return 0x0000;
    }
    uint16_t brightness = (iter & 1) << 4 | 0xF;
    uint16_t r = (iter & (1 << 3)) ? brightness : 0x0;
    uint16_t g = (iter & (1 << 2)) ? brightness : 0x0;
    uint16_t b = (iter & (1 << 1)) ? brightness : 0x0;
    return ((r & 0x1f) << 11) | ((g & 0x1f) << 6) | ((b & 0x1f));
}

rgb565 iter_to_colour1(uint16_t iter, uint16_t n_max)
{
    if (iter == n_max)
    {
        return 0x0000;
    }
    uint16_t brightness = ((iter & 0x78) >> 2) ^ 0x1F;
    uint16_t r = (iter & (1 << 2)) ? brightness : 0x0;
    uint16_t g = (iter & (1 << 1)) ? brightness : 0x0;
    uint16_t b = (iter & (1 << 0)) ? brightness : 0x0;
    return ((r & 0xf) << 12) | ((g & 0xf) << 7) | ((b & 0xf) << 1);
}
