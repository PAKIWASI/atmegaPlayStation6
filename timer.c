#include "timer.h"
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


// global timer
static volatile u32 ms;


void timer_init(void)
{
    // CTC mode, prescaler 64: 16 MHz / 64 = 250 kHz, so 250 counts = 1 ms
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A  = 249;
    TIMSK0 = (1 << OCIE0A);
}

// what to do during the interupt:
// just increment the counter
ISR(TIMER0_COMPA_vect)
{
    ms++;
}

u32 millis(void)
{
    u32 t;
    // A 32-bit read takes 4 instructions on an 8-bit CPU. Without this block the
    // ISR can update the value halfway through the read and return garbage
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        t = ms;
    }
    return t;
}
