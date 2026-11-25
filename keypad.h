/*
  4x4 membrane keypad, debounced, with an event queue
 
  Wiring (keypad pins left to right, looking at the front):
    pins 1-4 = rows R1..R4 -> D4..D7 (PD4..PD7)
    pins 5-8 = cols C1..C4 -> A0..A3 (PC0..PC3)
  No external resistors: internal pull-ups are used

  Layout:  1 2 3 A
           4 5 6 B
           7 8 9 C
           * 0 # D
 */
#ifndef KEYPAD_H
#define KEYPAD_H

#include "common.h"


void keypad_init(void);
// call every ~10 ms
void keypad_scan(void);
// next newly pressed key, or 0 if none
char keypad_get(void);
// held state, for continuous movement
u8   keypad_is_down(char key);
// drop queued events (e.g. on game switch)
void keypad_flush(void);



#endif // KEYPAD_H
