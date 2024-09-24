#include "utoa.h"

#include <spr.h>
#include <stdio.h>
#include <vga.h>

int main()
{
    vga_clear();

    const char digits[] = "0123456789ABCDEFGHIJ";
    const unsigned int base = sizeof(digits) - 1;

    char buf[3] = "";
    const unsigned int bufsz = sizeof(buf);

    for (unsigned int number = 0; number <= 100; ++number)
    {
        // Some arbitrary big enough buffer, we could compute the actual
        // required number of digits if we wanted to
        char buf[16] = {};
        const unsigned int bufsz = sizeof(buf);

        unsigned int num_chars = utoa(number, buf, bufsz, base, digits);
        printf("%u -> ", number);
        if (num_chars == 0)
        {
            printf("ERROR\n");
        }
        else
        {
            printf("\"%s\" (digits: %u)\n", buf, num_chars);
        }
    }
}
