/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>

const int read_buffer_size = 100;

MyReader::MyReader(const std::string& name, Writer& mywriter)
    : thewriter(mywriter) {
    this->in.open(name);
}

MyReader::~MyReader() {
    this->in.close();
}

void MyReader::run() {
    std::string ingest = new char[read_buffer_size]();
    while(!this->in.eof()){
        std::getline(this->in, ingest);
        this->thewriter.append(ingest);
    }
}
