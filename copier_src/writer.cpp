/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
Writer::Writer(const std::string& name, write_queue_t& write_queue, queue_slot_mutexs_t& queue_slot_mutexs, queue_wait_conds_t& queue_wait_conds) {
    this->out.open(name);
    this->queue = std::deque<std::string>();
}

Writer::~Writer() {
    this->out.close();
}

void Writer::run() {
    while( this->queue.back() != this->queue.front() ) {
        this->out.write(this->queue.front().c_str(), this->queue.front().size());
        this->out.write("\n", 1);
        this->queue.pop_front();
    }
    this->out.write(this->queue.front().c_str(), this->queue.front().size());
    this->queue.pop_front();
}

void Writer::append(const std::string& line) {
    this->queue.emplace_back(line);
}
