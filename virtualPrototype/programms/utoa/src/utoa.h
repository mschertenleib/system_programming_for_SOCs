#ifndef UTOA_H
#define UTOA_H

/*
Converts a given unsigned int number to string for the given base.

@note requires bufsz > 1 and base > 1.
@note appends NUL character at the end of the output.
@note writes buf[0] = 0 in case of failure.

@return int 0 in case of overflow or invalid argument, or number of written
characters on case of success (excluding NUL).
*/
unsigned int utoa(
    // Number to convert
    unsigned int number,
    // Output buffer
    char *buf,
    // Size of the output buffer
    unsigned int bufsz,
    // Base (also length of digits)
    unsigned int base,
    // Digits in the base
    const char *digits);

#endif