/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include <fstream>
#include <iostream>
#include <string>
#include <deque>
#ifndef WRITER
#define WRITER
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
    void append(const std::string& line);
    void set_timer_enabled(bool timer_enabled){this->timer_enabled = timer_enabled;}

   private:
    std::ofstream out;
    std::deque<std::string> queue;
    bool timer_enabled = false;
};
#endif
