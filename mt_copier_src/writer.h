/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include <fstream>
#include <iostream>
#include <string>
#include <deque>
#include <map>
#include <cmath>
#include <queue>
#include <set>

#ifndef WRITER
#define WRITER

const int MAX_SUPPORTED_THREADS = 100;
// Must be a power of 2. Used to reduce modulo operators in loops.
// Can't set via function as arrays are static.
const int ORDER_ARRAY_SIZE = 256;
const int ORDER_ACCESS_BITMASK = ORDER_ARRAY_SIZE - 1;
// Number of characters to try and read at a time.
const int READ_CHUNK_SIZE = 1024;


struct file_line {
    char line[READ_CHUNK_SIZE] = {'\0'};
    int line_number = 0;
};

typedef file_line* file_line_ptr;

struct file_line_sort{
    bool operator()(const file_line& lhs, const file_line& rhs) const {
        return lhs.line_number < rhs.line_number;
    };
};

typedef std::set<file_line, file_line_sort> write_queue_t;

struct write_thread_parameters {
    std::ofstream& outfile;
    write_queue_t& write_queue;
    pthread_mutex_t& queue_mutex;

    bool& eof_reached;
    int& next_line_num_read;
    int num_threads;
    int& total_lines;
    int& next_line_num_write;
    bool timer_enabled;

    std::_Rb_tree_const_iterator<file_line> write_queue_iter;
    std::_Rb_tree_const_iterator<file_line> write_queue_iter_end;

};

class Writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    Writer(const std::string& name, write_queue_t& write_queue, pthread_mutex_t& queue_mutex);
    ~Writer();
    /**
     * @brief Starts running writer threads
     * 
     * @param num_threads Number of threads to start.
     **/
    void run(int num_threads, bool enable_timer);
    /**
     * @brief Waits for all threads to finish execution.
     * 
     * @param num_threads Number of threads started to join
     */
    void join_threads(int num_threads);
    /**
     * @brief Sets eof_reached to true. 
     * Indicates that the file has finished reading and no
     * more lines will be added to the queue.
     */
    void read_finished(int total_lines);

   private:
    std::ofstream out;
    write_queue_t& write_queue;
    pthread_mutex_t& queue_mutex;
    int current_line;
    bool eof_reached;
    int total_lines;
    int lines_written;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    write_thread_parameters* thread_config;
};

#endif
