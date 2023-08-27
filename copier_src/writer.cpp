/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
Writer::Writer(const std::string& name) {
    this->out.open(name);
    this->queue = std::deque<std::string>();
}

Writer::~Writer() {
    this->out.close();
}

void Writer::run() {
    clock_t queue_read_start;
    clock_t queue_read_end;
    clock_t line_write_end;
    if(this->timer_enabled){queue_read_start = clock();}
    while( this->queue.back() != this->queue.front() ) {
        this->out.write(this->queue.front().c_str(), this->queue.front().length());
        this->out.write("\n", 1);
        this->queue.pop_front();
        if(this->timer_enabled){queue_read_end = clock();}
        if(this->timer_enabled){
            line_write_end = clock();
            this->timing_info.emplace_back(write_loop_time_info({
                (clock_t)0,
                queue_read_end - queue_read_start,
                (clock_t)0,
                line_write_end - queue_read_end
            }));
            queue_read_start = clock();
        }

    }
    this->out.write(this->queue.front().c_str(), this->queue.front().size());
    this->queue.pop_front();
}

void Writer::append(const std::string& line) {
    this->queue.emplace_back(line);
}
