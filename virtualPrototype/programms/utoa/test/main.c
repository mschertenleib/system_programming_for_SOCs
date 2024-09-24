#include "../src/utoa.h"

#include <stdio.h>

int main()
{
    const char digits[] = "0123456789ABCDEFGHIJ";
    // const char digits[] = "0123456789";
    const unsigned int base = sizeof(digits) - 1;

    for (unsigned int number = 0; number <= 100; ++number)
    {
        char buf[10] = {};
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
