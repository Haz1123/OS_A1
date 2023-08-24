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

struct file_line {
    std::string line;
    int line_number;
};

class writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    writer(const std::string& name);
    ~writer();
    /**
     * does the actual writing
     **/
    void run(int num_threads);
    /**
     * appends a line from the file read in to be output ... needed by the
     * reader class
     **/
    void append(file_line);

    /**
     * @brief Sets the value of eof_reached.
     * Indicates that the file has finished reading.
     * 
     * @param eof_reached 
     */
    void set_eof(bool eof_reached);

   private:
    std::ofstream out;
    std::deque<file_line> queue;
    int current_line;
    bool eof_reached;
};

#endif
