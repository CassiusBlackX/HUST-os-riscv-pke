#ifndef _SCHED_H_
#define _SCHED_H_

#include "process.h"

//length of a time slice, in number of ticks
#define TIME_SLICE_LEN  2

void insert_to_ready_queue( process* proc );
/// added at lab3 challenge 1
void insert_to_blocked_queue(process* proc);
/// added at lab3 challenge 1 wake up
void wake_up(process* proc);
void schedule();

#endif
