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
    MyReader(const std::string& name, Writer& mywriter);
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
    int read_lines;
    Writer& thewriter;
    int queued_lines;
    pthread_t threads[MAX_SUPPORTED_THREADS];
    read_thread_params* thread_parameters;
    int finished_thread_count;
};
#endif
