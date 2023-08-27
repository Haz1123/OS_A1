#include <time.h>
#include <string>
#include <numeric>
#include <vector>

#ifndef TIMER_H
#define TIMER_H

struct write_loop_time_info {
    clock_t tp_queue_mutex_time;
    clock_t tp_line_read_time;
    clock_t tp_write_order_time;
    clock_t tp_write_time;
};

struct read_loop_time_info {
    clock_t readfile_mutex_time;
    clock_t tp_line_read_time;
    clock_t local_queue_insertion_time;
};

struct read_general_time_info {
    clock_t queue_mutex_wait;
    clock_t queue_merge_time;
};

class time_helper {
    public:
        static double sum_times(std::vector<clock_t> time_list) {
            return std::accumulate(time_list.begin(), time_list.end(), 0.0);
        }
        static double sum_times_real(std::vector<clock_t> time_list) {
            return time_helper::sum_times(time_list) / (double) CLOCKS_PER_SEC;
        }
};

#endif  //!TIMER_H
