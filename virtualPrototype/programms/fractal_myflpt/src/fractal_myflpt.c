#include "fractal_myflpt.h"
#include <swap.h>

#include <printf.h>

my_float float_to_my_float(float f)
{
  union
  {
    float f;
    struct
    {
      // IEEE 754
      uint32_t sign : 1;
      uint32_t exponent : 8;
      uint32_t mantissa : 23;
    } parts;
  } s = {.f = f};

  uint32_t exponent = (uint32_t)((int32_t)s.parts.exponent - 127 + EXPONENT_EXCESS);
  uint32_t mantissa = s.parts.mantissa << (MANTISSA_BITS - 23);
  my_float result = {.sign = s.parts.sign, .exponent = exponent, .mantissa = mantissa};
  return result;
}

//! \brief  Mandelbrot fractal point calculation function
//! \param  cx    x-coordinate
//! \param  cy    y-coordinate
//! \param  n_max maximum number of iterations
//! \return       number of performed iterations at coordinate (cx, cy)
uint16_t calc_mandelbrot_point_soft(my_float cx, my_float cy, uint16_t n_max)
{
  my_float x = cx;
  my_float y = cy;
  uint16_t n = 0;
  my_float xx, yy, two_xy;
  const my_float two = {.sign = 0, .exponent = 1 + EXPONENT_EXCESS, .mantissa = 0};
  const my_float four = {.sign = 0, .exponent = 2 + EXPONENT_EXCESS, .mantissa = 0};
  do
  {
    xx = mul(x, x);
    yy = mul(y, y);
    two_xy = mul(two, mul(x, y));

    x = add(sub(xx, yy), cx);
    y = add(two_xy, cy);
    ++n;
  } while (less_than(add(xx, yy), four) && (n < n_max));
  return n;
}

//! \brief  Map number of performed iterations to black and white
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return       colour
rgb565 iter_to_bw(uint16_t iter, uint16_t n_max)
{
  if (iter == n_max)
  {
    return 0x0000;
  }
  return 0xffff;
}

//! \brief  Map number of performed iterations to grayscale
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return       colour
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max)
{
  if (iter == n_max)
  {
    return 0x0000;
  }
  uint16_t brightness = iter & 0xf;
  return swap_u16(((brightness << 12) | ((brightness << 7) | brightness << 1)));
}

//! \brief Calculate binary logarithm for unsigned integer argument x
//! \note  For x equal 0, the function returns -1.
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
  n -= x >> 31;
  return 31 - n;
}

//! \brief  Map number of performed iterations to a colour
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return colour in rgb565 format little Endian (big Endian for openrisc)
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
  return swap_u16(((r & 0x1f) << 11) | ((g & 0x1f) << 6) | ((b & 0x1f)));
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
  return swap_u16(((r & 0xf) << 12) | ((g & 0xf) << 7) | ((b & 0xf) << 1));
}

//! \brief  Draw fractal into frame buffer
//! \param  width  width of frame buffer
//! \param  height height of frame buffer
//! \param  cfp_p  pointer to fractal function
//! \param  i2c_p  pointer to function mapping number of iterations to colour
//! \param  cx_0   start x-coordinate
//! \param  cy_0   start y-coordinate
//! \param  delta  increment for x- and y-coordinate
//! \param  n_max  maximum number of iterations
void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  my_float cx_0, my_float cy_0, my_float delta, uint16_t n_max)
{
  rgb565 *pixel = fbuf;
  my_float cy = cy_0;
  for (int k = 0; k < height; ++k)
  {
    my_float cx = cx_0;
    for (int i = 0; i < width; ++i)
    {
      uint16_t n_iter = (*cfp_p)(cx, cy, n_max);
      rgb565 colour = (*i2c_p)(n_iter, n_max);
      *(pixel++) = colour;
      cx = add(cx, delta);
    }
    cy = add(cy, delta);
  }
}
