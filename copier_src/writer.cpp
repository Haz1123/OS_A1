#include "writer.h"
#include <unistd.h>

pthread_mutex_t queue_lock;
pthread_mutex_t writefile_lock;
pthread_cond_t writefile_cond;
pthread_mutex_t eof_reached_lock;

writer::writer(const std::string& name) {
    this->out.open(name);
    this->queue = std::deque<file_line>();
    this->eof_reached = false;
    this->current_line = 0;
}

struct thread_common_params {
    std::deque<file_line> * write_queue;
    std::ofstream * outfile;
    bool * eof_reached;
    int * current_line;
};

struct thread_unique_params {
    u_long thread_id;
    thread_common_params *common_params;
};

void *write_thread(void *write_thread_params) {
    struct thread_unique_params *params = (struct thread_unique_params *)write_thread_params;
    do {
        while(pthread_mutex_trylock(&queue_lock) != 0  ) {
            sleep(1);
        };
            while( params->common_params->write_queue->size() > 0 ) {
                std::string line_str = params->common_params->write_queue->front().line;
                int line = params->common_params->write_queue->front().line_number;
                params->common_params->write_queue->pop_front();
                pthread_mutex_unlock(&queue_lock);
                pthread_mutex_lock(&writefile_lock);
                // Check and wait to ensure ordering in queue
                while(*params->common_params->current_line != line) {
                    std::cout << "Write order waiting...\n";
                    pthread_cond_wait(&writefile_cond, &writefile_lock);
                }
                params->common_params->outfile->write(line_str.c_str(), line_str.length());
                params->common_params->outfile->write("\n", 1);
                (*params->common_params->current_line)++;
                pthread_mutex_unlock(&writefile_lock);
                pthread_cond_broadcast(&writefile_cond);
                pthread_mutex_lock(&queue_lock);
            }
        pthread_mutex_unlock(&queue_lock);
        sleep(1);
    } while( *params->common_params->eof_reached == false);
    pthread_exit(NULL);
}

void writer::run(int num_threads) {
    struct thread_common_params params = {&this->queue, &this->out, &this->eof_reached, &this->current_line};
    pthread_t threads[100] = {0};
    // bool enable_write[100] = {true, false};
    // pthread_cond_t cond_wait[100];
    for(int i = 0; i < num_threads - 1; i++){
        struct thread_unique_params i_params = {threads[i], &params};
        pthread_create(&threads[i], NULL, write_thread, &i_params);
    }
    for( int i = 0; i < num_threads; i++ ) {
        pthread_join(threads[i], NULL);
    }
    this->out.close();
}

void writer::append(const file_line line) {
    while(pthread_mutex_trylock(&queue_lock) != 0) {
        sleep(1);
    }
    this->queue.emplace_back(line);
    pthread_mutex_unlock(&queue_lock);
}

void writer::set_eof(bool eof_reached) {
    while(pthread_mutex_trylock(&eof_reached_lock)){
        sleep(1);
    }
    this->eof_reached = eof_reached;
    pthread_mutex_unlock(&eof_reached_lock);
}