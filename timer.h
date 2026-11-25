
// timer.h - 1 ms system tick on Timer0

#ifndef TIMER_H
#define TIMER_H

#include "common.h"

// call before sei()
void timer_init(void);

// wraps after ~49 days
u32  millis(void);


#endif // TIMER_H
