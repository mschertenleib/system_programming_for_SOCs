#ifndef CUSTOM_FLOAT
#define CUSTOM_FLOAT

#include <stdint.h>

typedef uint32_t cfloat;

// our custom float is in 32bits and has the following layout:
// 1 bit for the sign       s
// 3 bits for the exponent  e
// 28 bits for the mantissa m

// the value is calculated as follows: (1 + m/2^28) * 2^(e - 3)
// max: 

cfloat float2c(float f)
{

}

cfloat mul(cfloat a, cfloat b)
{

}

cfloat add(cfloat a, cfloat b)
{

}

float c2float(cfloat a)
{

}

#endif // CUSTOM_FLOAT