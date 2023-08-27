/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "writer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "timer.h"

#ifndef READER
#define READER

struct read_thread_params {
    std::ifstream& infile;
    write_queue_t& write_queue;
    pthread_mutex_t& queue_mutex;
    int& finished_read_lines;
    int& queued_lines;
    bool timer;
    std::vector<read_loop_time_info>& loop_time_info;
    std::vector<read_general_time_info>& general_time_info;
};

class MyReader {
   public:
    /* create a reader that reads each line of the file and appends it to the
     * writer's queue
     */
    MyReader(const std::string& name, Writer& mywriter, write_queue_t& write_queue, pthread_mutex_t& queue_lock);
    ~MyReader();
    /* perform the reading from the file */
    void run(int num_threads);
    // Enable timing code.
    void set_timer_enabled(bool timer_enabled) {this->timer = timer_enabled;};
    /**
     * @brief Waits for all threads to finish execution.
     * 
     * @param num_threads 
     */
    void join_threads(int num_threads);

    std::vector<read_loop_time_info> loop_time_info;
    std::vector<read_general_time_info> general_time_info;
   private:
    std::ifstream in;
    Writer& thewriter;
    write_queue_t& write_queue;
    pthread_mutex_t queue_mutex;
    int read_lines;
    int queued_lines;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    read_thread_params* thread_parameters;
    int finished_thread_count;
    bool timer;
    
};
#endif
