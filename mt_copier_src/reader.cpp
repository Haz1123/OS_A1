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

MyReader::MyReader(const std::string& name, Writer& mywriter, write_queue_t& write_queue, queue_slot_mutexs_t& queue_slot_mutexs, queue_wait_conds_t& queue_wait_conds)
    : thewriter(mywriter), 
    write_queue(write_queue), 
    queue_mutexes(queue_slot_mutexs), 
    queue_slot_conds(queue_wait_conds)
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
    pthread_mutex_lock(&read_lock);
    while(!params->infile.eof()){
        file_line* ingest = new file_line();
        std::getline(params->infile, ingest->line);
        ingest->line_number = params->current_line;
        ingest->read = true;
        ingest->written = false;
        params->current_line++;
        if((params->current_line % 1000) == 0) {
            std::cout << "READ:" << params->current_line << "\n";
        }
        pthread_mutex_unlock(&read_lock);
        
        pthread_mutex_lock(&params->queue_slot_mutexs[ingest->line_number & 255]);
        // Validate line can be written to
        // Also validate line ordering is correct
        while(params->write_queue[ingest->line_number & 255]->read == true || ingest->line_number > params->write_queue[ingest->line_number & 255]->line_number + 256){
            if(ingest->line_number > params->write_queue[ingest->line_number & 255]->line_number + 256){
                std::cout<<"I'm out of order to queue! Wait for" << (ingest->line_number & 255) << "\n";
            } else {
                std::cout << "I'm waiting for something to write!" << ingest->line_number << "\n";
            }
            pthread_cond_wait(&params->queue_wait_conds[ingest->line_number & 255], &params->queue_slot_mutexs[ingest->line_number & 255]);
        }
        params->write_queue[ingest->line_number & 255] = ingest;
        // Wake up writer/other readers
        pthread_cond_broadcast(&params->queue_wait_conds[ingest->line_number & 255]);
        // Wake up next reader thread?
        pthread_cond_broadcast(&params->queue_wait_conds[(ingest->line_number + 1) & 255]);
        pthread_mutex_unlock(&params->queue_slot_mutexs[ingest->line_number & 255]);
        // Lock read ahead of loop
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);

    pthread_mutex_lock(&finished_count_lock);
    params->finished_thread_count++;
    // Check if this thread is the last thread to finish writing.
    if(params->finished_thread_count == params->num_threads) {
        std:: cout << "Last thread exit.\n" << "Lines total:" << params->current_line << "\n";
        pthread_mutex_lock(&writer_lock);
        params->writer.read_finished(params->current_line);
        pthread_mutex_unlock(&writer_lock);
    } else {
        std::cout << "Thread:" << params->finished_thread_count << ":" << params->num_threads << " exit\n";
    };
    pthread_mutex_unlock(&finished_count_lock);

    pthread_exit(NULL);
}

void MyReader::run(int num_threads) {
    pthread_mutex_init(&writer_lock, NULL);
    this->thread_parameters = new read_thread_params(
        {
            this->in, 
            this->write_queue, 
            this->queue_mutexes, 
            this->queue_slot_conds,

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