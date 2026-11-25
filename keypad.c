#include "keypad.h"
#include "common.h"

#include <avr/io.h>
#include <util/delay.h>


#define ROW_MASK  0xF0   // PD4..PD7
#define COL_MASK  0x0F   // PC0..PC3
#define QUEUE_SZ  8      // power of two so the index wraps with a mask

static const char keymap[16] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

static u16 last_raw;   // previous scan
static u16 stable;     // debounced state, bit (row*4 + col)
static char     queue[QUEUE_SZ];
static u8 q_head, q_tail;


void keypad_init(void)
{
    // Rows idle as inputs with pull-up. A row is driven low only while scanned
    // so two keys pressed in one column never short a high output to a low one
    DDRD  &= ~ROW_MASK;
    PORTD |=  ROW_MASK;
    DDRC  &= ~COL_MASK;
    PORTC |=  COL_MASK;
}

static void push(char k)
{
    u8 next = (q_head + 1) & (QUEUE_SZ - 1);
    if (next != q_tail) {           // drop the key if the queue is full
        queue[q_head] = k;
        q_head = next;
    }
}

void keypad_scan(void)
{
    u16 raw = 0;

    for (u8 r = 0; r < 4; r++) {
        u8 bit = 1 << (PD4 + r);
        PORTD &= ~bit;              // pull-up off first, then output low
        DDRD  |=  bit;
        _delay_us(5);               // let the line settle through the membrane
        u8 cols = (~PINC) & COL_MASK;   // pressed = pulled low
        raw |= (u16)cols << (r * 4);
        DDRD  &= ~bit;              // back to input with pull-up 
        PORTD |=  bit;
    }

    // Debounce: accept a state only if two scans (10 ms apart) agree
    if (raw == last_raw) {
        u16 pressed = raw & ~stable;
        stable = raw;
        for (u8 i = 0; i < 16; i++)
            if (pressed & (1u << i)) push(keymap[i]);
    }
    last_raw = raw;
}

char keypad_get(void)
{
    if (q_head == q_tail) return 0;
    char k = queue[q_tail];
    q_tail = (q_tail + 1) & (QUEUE_SZ - 1);
    return k;
}

u8 keypad_is_down(char key)
{
    for (u8 i = 0; i < 16; i++)
        if (keymap[i] == key) return (stable >> i) & 1;
    return 0;
}

void keypad_flush(void)
{
    q_tail = q_head;
}


