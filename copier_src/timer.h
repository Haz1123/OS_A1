#include <time.h>
#include <string>

#ifndef TIMER_H
#define TIMER_H

struct loop_time_info {
    clock_t tp_loop_start;
    clock_t tp_queue_lock;
    clock_t tp_line_read;
    clock_t tp_write_lock;
    clock_t tp_line_order;
    clock_t tp_line_write;
    clock_t tp_loop_finish;
};

#endif  //!TIMER_H
