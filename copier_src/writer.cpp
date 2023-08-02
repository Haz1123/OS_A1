/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
writer::writer(const std::string& name) {
    this->out.open(name);
    this->queue = std::deque<std::string>();
}

void writer::run() {
    while( !this->queue.empty() ) {
        // TODO:    Check how buffer impacts the end of 
        //          the write. Can't have extra bytes
        //          at end of file.
        this->out.write(this->queue.front().c_str(), 10);
        this->queue.pop_front();
    }
}

void writer::append(const std::string& line) {
    this->queue.emplace_back(line);
}
