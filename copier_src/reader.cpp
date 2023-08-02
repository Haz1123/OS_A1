/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>

const int read_buffer_size = 10;

reader::reader(const std::string& name, writer& mywriter)
    : thewriter(mywriter) {
    // TODO: Init ifstream properly
    //this->in = new ifstream();
    this->in.open(name);
}

void reader::run() {
    char* ingest = new char[read_buffer_size]();
    this->in.read(ingest, read_buffer_size);
    //TODO: Check how using a fixed is impacting reads,
    //      check if last string needs to be trimmed
    this->thewriter.append(std::string(ingest));
}
