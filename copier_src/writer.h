/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "timer.h"

#ifndef WRITER
#define WRITER

// Number of characters to try and read at a time.
const int READ_CHUNK_SIZE = 1024;

struct file_line {
    char line[READ_CHUNK_SIZE];
};


class Writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    Writer(const std::string& name);
    ~Writer();
    /**
     * does the actual writing
     **/
    void run();
    /**
     * appends a line from the file read in to be output ... needed by the
     * reader class
     **/
    void append(file_line);

    void set_timer_enabled(bool timer_enabled){this->timer_enabled = timer_enabled;};

    std::vector<write_loop_time_info> timing_info;
   private:
    std::ofstream out;
    std::deque<file_line> queue;
    bool timer_enabled = false;
};
#endif
