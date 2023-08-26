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
pthread_cond_t read_order_condition[127];

MyReader::MyReader(const std::string& name, Writer& mywriter)
    : thewriter(mywriter) {
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
    pthread_mutex_lock(&read_lock);
    while(!params->infile.eof()){
        file_line ingest;
        std::getline(params->infile, ingest.line);
        std::cout << ingest.line << "\n";
        ingest.line_number = params->current_line;
        params->current_line++;
        if((params->current_line % 1000) == 0) {
            std::cout << "READ:" << params->current_line << "\n";
        }
        pthread_mutex_unlock(&read_lock);

        pthread_mutex_lock(&writer_lock);
        // Check if the current line will write out of order
        while(ingest.line_number > (params->writer.checkQueueInsert(ingest.line_number) + 128)){
            // Tried to insert in wrong order.
            std::cout << "order incorrect!\n";
            pthread_cond_wait(&read_order_condition[ingest.line_number & 127], &writer_lock);
        }
        params->writer.append(ingest);
        params->queued_lines++;
        pthread_cond_broadcast(&read_order_condition[(ingest.line_number) & 127]);
        pthread_mutex_unlock(&writer_lock);
        
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);
    


    pthread_mutex_lock(&finished_count_lock);
    params->finished_thread_count++;
    // Check if this thread is the last thread to finish writing.
    if(params->finished_thread_count == params->num_threads) {
        pthread_mutex_lock(&writer_lock);
        params->writer.read_finished(params->queued_lines);
        pthread_mutex_unlock(&writer_lock);
        std:: cout << "Last thread exit.\n";
    } else {
        std::cout << "Thread:" << params->finished_thread_count << ":" << params->num_threads << " exit\n";
    };
    pthread_mutex_unlock(&finished_count_lock);

    pthread_exit(NULL);
}

void MyReader::run(int num_threads) {
    this->thread_parameters = new read_thread_params({this->in, this->read_lines, this->thewriter, this->queued_lines, num_threads, this->finished_thread_count});
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