#include "utoa.h"

unsigned int utoa(unsigned int number,
                  char *buf,
                  unsigned int bufsz,
                  unsigned int base,
                  const char *digits)
{
    if (bufsz <= 1 || base <= 1)
    {
        buf[0] = 0;
        return 0;
    }

    // Find the required number of digits
    unsigned int power = base;
    unsigned int num_digits = 1;
    while (number >= power)
    {
        power *= base;
        ++num_digits;
    }

    if (num_digits + 1 > bufsz)
    {
        buf[0] = 0;
        return 0;
    }

    for (unsigned int i = 0; i < num_digits; ++i)
    {
        buf[num_digits - i - 1] = digits[number % base];
        number /= base;
        if (number == 0)
        {
            break;
        }
    }

    return num_digits;
}
