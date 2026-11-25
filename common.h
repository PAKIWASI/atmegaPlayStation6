
// common stuff for all code

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// i dont wanna write this long ass type!

typedef uint8_t  u8;

typedef uint16_t u16;

typedef uint32_t u32;



// TODO: is it fine to define this in a header?
#ifndef F_CPU
    #define F_CPU 16000000UL
#endif


#endif // COMMON_H
