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

const int MAX_SUPPORTED_THREADS = 100;

struct file_line {
    std::string line;
    int line_number;
};

struct write_thread_parameters {
    std::deque<file_line>& write_queue;
    std::ofstream& outfile;
    bool& eof_reached;
    int& current_line;
    int num_threads;
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
    void run(int num_threads);
    /**
     * @brief Waits for all threads to finish execution.
     * 
     * @param num_threads 
     */
    void join_threads(int num_threads);
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
    pthread_t threads[MAX_SUPPORTED_THREADS];
    write_thread_parameters* thread_config;
};

#endif
