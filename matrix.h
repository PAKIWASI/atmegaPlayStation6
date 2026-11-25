/*
  matrix.h: MAX7219 32x8 (FC-16 style, 4 chained 8x8) framebuffer driver
 
  Wiring (hardware SPI, Arduino Uno pin names):
    VCC -> 5V  GND -> GND
    DIN -> D11 (PB3, MOSI)
    CLK -> D13 (PB5, SCK)
    CS  -> D10 (PB2, SS)
 
  Coordinates: x = 0..31 left to right, y = 0..7 top to bottom.
  Draw into the RAM framebuffer, then call matrix_flush() once per frame.
*/

#ifndef MATRIX_H
#define MATRIX_H

#incldue "common.h"


// how many blocks we have of 8x8 dot display
#define MATRIX_MODULES  4
// the screen is ultra wide
#define MATRIX_W        (MATRIX_MODULES * 8)
// the height of screen
#define MATRIX_H        8


// intensity 0..15
void    matrix_init(u8 intensity);

void    matrix_set_intensity(u8 intensity);

void    matrix_clear(void);

void    matrix_fill(void);
// clips off-screen
void    matrix_set(u8 x, u8 y, u8 on);
// 0 if off-screen
u8 matrix_get(u8 x, u8 y);

void    matrix_toggle(u8 x, u8 y);
// sends only if something changed
void    matrix_flush(void);



#endif // MATRIX_H
