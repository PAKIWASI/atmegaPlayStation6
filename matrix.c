#include "matrix.h"
#include <avr/io.h>
#include <string.h>

// MAX7219 registers
#define REG_DIGIT0      0x01    // 0x01..0x08 = rows 0..7
#define REG_DECODE      0x09
#define REG_INTENSITY   0x0A
#define REG_SCANLIMIT   0x0B
#define REG_SHUTDOWN    0x0C
#define REG_TEST        0x0F

#define CS_LOW()   (PORTB &= ~(1 << PB2))
#define CS_HIGH()  (PORTB |=  (1 << PB2))

// The unsigned cast rejects negatives and too-large values in one compare
#define OUT_OF_BOUNDS(x, y) ((u8)(x) >= MATRIX_W || (u8)(y) >= MATRIX_H)


// TODO: why global?
// fb[row][module]: bit 7 = leftmost pixel of that module. 32 bytes total
static u8 fb[MATRIX_H][MATRIX_MODULES];
static u8 dirty;


static inline void spi_tx(u8 b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)));
}

// Write the same register/value to every chip in the chain
static void max_all(u8 reg, u8 val)
{
    CS_LOW();
    for (u8 i = 0; i < MATRIX_MODULES; i++) {
        spi_tx(reg);
        spi_tx(val);
    }
    CS_HIGH();   // rising edge latches all chips at once
}

void matrix_init(u8 intensity)
{
    // SS (PB2) must be an output, or a low level on it drops SPI out of master mode
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
    CS_HIGH();

    SPCR = (1 << SPE) | (1 << MSTR);   // SPI mode 0, MSB first
    SPSR = (1 << SPI2X);               // F_CPU/2 = 8 MHz, MAX7219 max is 10 MHz

    max_all(REG_TEST, 0x00);
    max_all(REG_DECODE, 0x01);         // raw bit patterns, no BCD font
    max_all(REG_SCANLIMIT, 0x07);      // scan all 8 rows
    max_all(REG_INTENSITY, intensity & 0x0F);
    max_all(REG_SHUTDOWN, 0x01);       // normal operation

    matrix_clear();
    matrix_flush();
}

void matrix_set_intensity(u8 intensity)
{
    max_all(REG_INTENSITY, intensity & 0x0F);
}

void matrix_clear(void)
{
    memset(fb, 0x00, sizeof fb);
    dirty = 1;
}

void matrix_fill(void)
{
    memset(fb, 0xFF, sizeof fb);
    dirty = 1;
}


void matrix_set(u8 x, u8 y, u8 on)
{
    if (OUT_OF_BOUNDS(x, y)) return;
    u8 mask = 0x80 >> (x & 7);
    u8 *p = &fb[y][x >> 3];
    if (on) *p |= mask;
    else    *p &= ~mask;
    dirty = 1;
}

u8 matrix_get(u8 x, u8 y)
{
    if (OUT_OF_BOUNDS(x, y)) return 0;
    return (fb[y][x >> 3] & (0x80 >> (x & 7))) ? 1 : 0;
}

void matrix_toggle(u8 x, u8 y)
{
    matrix_set(x, y, !matrix_get(x, y));
}

/*
  One SPI frame per row: 4 x (register, data) = 8 bytes.
  Bytes shift through the chain, so the FIRST pair sent ends up in the
  chip FARTHEST from DIN. On FC-16 boards that is the leftmost module.
  Full flush = 64 bytes at 8 MHz, roughly 80 us.
 */
void matrix_flush(void)
{
    if (!dirty) return;

    for (u8 r = 0; r < MATRIX_H; r++) {
        CS_LOW();
        for (u8 m = 0; m < MATRIX_MODULES; m++) {
            u8 mod = m;
            spi_tx(REG_DIGIT0 + r);
            spi_tx(fb[r][mod]);
        }
        CS_HIGH();
    }
    dirty = 0;
}




