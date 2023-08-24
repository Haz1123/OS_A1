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
pthread_cond_t read_order_condition[MAX_SUPPORTED_THREADS];

MyReader::MyReader(const std::string& name, Writer& mywriter)
    : thewriter(mywriter) {
    this->in = std::ifstream(name);
    this->read_lines= 0;
    this->queued_lines = 0;
}

MyReader::~MyReader() {
    this->in.close();
    delete this->thread_parameters;
}

void *read_thread(void *read_thread_params) {
    struct read_thread_params *params = (struct read_thread_params *)read_thread_params;
    pthread_mutex_lock(&read_lock);
    while(!params->infile.eof()){
        file_line ingest;
        std::getline(params->infile, ingest.line);
        ingest.line_number = params->current_line;
        params->current_line++;
        pthread_mutex_unlock(&read_lock);
        pthread_mutex_lock(&writer_lock);
        // Check and wait to ensure ordering in queue.
        while(ingest.line_number != params->queued_lines){
            pthread_cond_wait(&read_order_condition[(ingest.line_number % params->num_threads)], &writer_lock);
        }
        params->writer.append(ingest);
        params->queued_lines++;
        pthread_cond_signal(&read_order_condition[(ingest.line_number + 1) % params->num_threads]);
        pthread_mutex_unlock(&writer_lock);
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);
    while(pthread_mutex_trylock(&writer_lock)) {
        sleep(1);
    }
    params->writer.set_eof(true);
    pthread_mutex_unlock(&writer_lock);
    pthread_exit(NULL);
}

void MyReader::run(int num_threads) {
    this->thread_parameters = new read_thread_params({this->in, this->read_lines, this->thewriter, this->queued_lines, num_threads});
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