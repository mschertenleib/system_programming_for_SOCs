#include <stdio.h>
#include <stdint.h>

#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b

static float minv = 1e6;
static float maxv = -1e6;

uint16_t calc_mandelbrot_point(float cx, float cy, uint16_t n_max)
{
    float x = cx;
    float y = cy;
    uint16_t n = 0;
    float xx, yy, two_xy;

    do
    {
        xx = x * x;         // 4
        yy = y * y;         // 4
        two_xy = 2 * x * y; // -8 to 8

        x = xx - yy + cx; // -6 to 6
        y = two_xy + cy;  // -10 to 10
        ++n;

        minv = min(x, minv);
        minv = min(y, minv);
        minv = min(two_xy, minv);

        maxv = max(x, maxv);
        maxv = max(y, maxv);
        maxv = max(xx, maxv);
        maxv = max(yy, maxv);
        maxv = max(xx + yy, maxv);
        maxv = max(two_xy, maxv);

    } while (((xx + yy) < 4) && (n < n_max));
    return n;
}

void draw_fractal(int width, int height, float cx_0, float cy_0, float delta, uint16_t n_max)
{
    float cy = cy_0;
    for (int k = 0; k < height; ++k)
    {
        float cx = cx_0;
        for (int i = 0; i < width; ++i)
        {
            uint16_t n_iter = calc_mandelbrot_point(cx, cy, n_max);
            cx += delta;
        }
        cy += delta;
    }
}

int main()
{
    draw_fractal(512, 512, -2.0, -1.5, 3.0 / 512, 64);
    printf("%f %f\n", minv, maxv);
    return 0;
}
