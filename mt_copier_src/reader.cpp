/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t read_lock;
pthread_mutex_t writer_lock;
pthread_mutex_t finished_count_lock;

MyReader::MyReader(const std::string& name, Writer& mywriter, write_queue_t& write_queue, pthread_mutex_t& queue_lock)
    : thewriter(mywriter), 
    write_queue(write_queue), 
    queue_mutex(queue_lock)
     {
    this->in = std::ifstream(name);
    this->read_lines= 0;
    this->queued_lines = 0;
    this->finished_thread_count = 0;
}

MyReader::~MyReader() {
    this->in.close();
    delete this->thread_parameters;
}

void *read_thread(void *read_thread_params) {
    struct read_thread_params *params = (struct read_thread_params *)read_thread_params;
    file_line_ptr ingest = new file_line();
    pthread_mutex_lock(&read_lock);
    while(!params->infile.eof()){
        std::getline(params->infile, ingest->line);
        ingest->line_number = params->finished_read_lines;
        params->finished_read_lines++;
        pthread_mutex_unlock(&read_lock);
        if((ingest->line_number % 100000) == 0){
            std::cout << "READ:" << ingest->line_number << "\n";
        }
        ingest->line.append("\n");
        pthread_mutex_lock(&params->queue_mutex);
        params->write_queue.push_back(ingest);
        pthread_mutex_unlock(&params->queue_mutex);
        ingest = new file_line;
        // Lock read ahead of loop
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);

    pthread_mutex_lock(&finished_count_lock);
    params->finished_thread_count++;
    // Check if this thread is the last thread to finish writing.
    if(params->finished_thread_count == params->num_threads) {
        pthread_mutex_lock(&writer_lock);
        params->writer.read_finished(params->finished_read_lines);
        pthread_mutex_unlock(&writer_lock);
    }
    pthread_mutex_unlock(&finished_count_lock);

    pthread_exit(NULL);
}

void MyReader::run(int num_threads) {
    pthread_mutex_init(&writer_lock, NULL);
    this->thread_parameters = new read_thread_params(
        {
            this->in, 
            this->write_queue, 
            this->queue_mutex,

            this->read_lines, 
            this->thewriter, 
            this->queued_lines, 
            num_threads, 
            this->finished_thread_count
        }
    );
    for(int i = 0; i < num_threads; i++){
        pthread_create(&this->threads[i], NULL, read_thread, this->thread_parameters);
    }
}

void MyReader::join_threads(int num_threads) {
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(this->threads[i], NULL);
    }
}