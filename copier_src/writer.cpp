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

void writer::run() {}

void writer::append(const std::string& line) {}
