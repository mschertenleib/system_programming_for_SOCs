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
            (void)n_iter;
            cx += delta;
        }
        cy += delta;
    }
}

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

int main()
{
    // draw_fractal(512, 512, -2.0, -1.5, 3.0 / 512, 64);

    const fxpt_7_25 a = float_to_fix(-1.5);
    const fxpt_7_25 b = float_to_fix(2.75);
    printf("0x%08x  %f\n", a, fix_to_float(a));
    printf("0x%08x  %f\n", b, fix_to_float(b));

    const fxpt_7_25 c = mul(a, b);
    printf("0x%08x  %f\n", c, fix_to_float(c));

    const fxpt_7_25 d = div(a, b);
    printf("0x%08x  %f\n", d, fix_to_float(d));

    return 0;
}
