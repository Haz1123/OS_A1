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
class writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    writer(const std::string& name);
    /**
     * does the actual writing
     **/
    void run();
    /**
     * appends a line from the file read in to be output ... needed by the
     * reader class
     **/
    void append(const std::string& line);

   private:
    std::ofstream out;
    std::deque<std::string> queue;
};
#endif
