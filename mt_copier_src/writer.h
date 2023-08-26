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
     * @brief Starts running writer threads
     * 
     * @param num_threads Number of threads to start.
     **/
    void run(int num_threads);
    /**
     * @brief Waits for all threads to finish execution.
     * 
     * @param num_threads Number of threads started to join
     */
    void join_threads(int num_threads);
    /**
     * @brief Adds a new line to the write queue.
     * It is assumed that .append is called in order.
     **/
    void append(file_line);

    /**
     * @brief Sets eof_reached to true. 
     * Indicates that the file has finished reading and no
     * more lines will be added to the queue.
     */
    void read_finished();

    std::deque<file_line> queue;
   private:
    std::ofstream out;
    int current_line;
    bool eof_reached;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    write_thread_parameters* thread_config;
};

#endif
