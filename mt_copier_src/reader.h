/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "writer.h"
#include <fstream>
#include <iostream>
#include <string>
#ifndef READER
#define READER

struct read_thread_params {
    std::ifstream& infile;
    write_queue_t& write_queue;
    queue_slot_mutexs_t& queue_slot_mutexs;
    queue_wait_conds_t& queue_wait_conds;

    int& current_line;
    Writer& writer;
    int& queued_lines;
    int num_threads;
    int& finished_thread_count;
};

class MyReader {
   public:
    /* create a reader that reads each line of the file and appends it to the
     * writer's queue
     */
    MyReader(const std::string& name, Writer& mywriter, write_queue_t& write_queue, queue_slot_mutexs_t& queue_slot_mutexs, queue_wait_conds_t& queue_wait_conds);
    ~MyReader();
    /* perform the reading from the file */
    void run(int num_threads);

    /**
     * @brief Waits for all threads to finish execution.
     * 
     * @param num_threads 
     */
    void join_threads(int num_threads);


   private:
    std::ifstream in;
    Writer& thewriter;
    write_queue_t& write_queue;
    queue_slot_mutexs_t& queue_mutexes;
    queue_wait_conds_t& queue_slot_conds;

    int read_lines;
    int queued_lines;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    read_thread_params* thread_parameters;
    int finished_thread_count;
};
#endif
