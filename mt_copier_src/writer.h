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
    int line_number = 0;
    bool read = false;
    bool written = true;
};

typedef file_line* file_line_ptr;
typedef file_line_ptr write_queue_t[256];
typedef pthread_mutex_t queue_slot_mutexs_t[256];
typedef pthread_cond_t queue_wait_conds_t[256];

struct write_thread_parameters {
    std::ofstream& outfile;
    write_queue_t& line_queues;
    queue_slot_mutexs_t& queue_slot_mutexs;
    queue_wait_conds_t& queue_wait_conds;
    bool& eof_reached;
    int& next_line_num_read;
    int num_threads;
    int& total_lines;
    int& next_line_num_write;
};

class Writer {
   public:
    /**
     * creates the writer instance that writes out to the file
     **/
    Writer(const std::string& name, write_queue_t& write_queue, queue_slot_mutexs_t& queue_slot_mutexs, queue_wait_conds_t& queue_wait_conds );
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
     * @brief Sets eof_reached to true. 
     * Indicates that the file has finished reading and no
     * more lines will be added to the queue.
     */
    void read_finished(int total_lines);

   private:
    std::ofstream out;
    write_queue_t& write_queue;
    queue_slot_mutexs_t& queue_mutexes;
    queue_wait_conds_t& queue_slot_conds;

    int current_line;
    bool eof_reached;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    write_thread_parameters* thread_config;
    int total_lines;
    int written_lines;
};

#endif
