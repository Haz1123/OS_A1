/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>

const int read_buffer_size = 100;

reader::reader(const std::string& name, writer& mywriter)
    : thewriter(mywriter) {
    this->in.open(name);
}

reader::~reader() {
    this->in.close();
}

void reader::run() {
    std::string ingest = new char[read_buffer_size]();
    while(!this->in.eof()){
        std::getline(this->in, ingest);
        this->thewriter.append(ingest);
    }
}
