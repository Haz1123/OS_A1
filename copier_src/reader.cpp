/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>
#include <pthread.h>
#include <unistd.h>

const int MAX_POSSIBLE_THREADS = 100;

pthread_mutex_t read_lock;
pthread_mutex_t writer_lock;
pthread_cond_t queue_cond;

reader::reader(const std::string& name, writer& mywriter)
    : thewriter(mywriter) {
    this->in.open(name);
    this->read_lines= 0;
    this->queued_lines = 0;
}

reader::~reader() {
    this->in.close();
}

struct read_thread_params {
    std::ifstream& infile;
    int * currentLine;
    writer& writefile;
    int * queued_lines;
};

void *read_thread(void *read_thread_params) {
    struct read_thread_params *x = (struct read_thread_params *)read_thread_params;
    while(pthread_mutex_trylock(&read_lock) != 0){
        sleep(1);
    }
    while(!x->infile.eof()){
        file_line ingest;
        std::getline(x->infile, ingest.line);
        ingest.line_number = *x->currentLine;
        (*x->currentLine)++;
        pthread_mutex_unlock(&read_lock);
        pthread_mutex_lock(&writer_lock);
        // Check and wait to ensure ordering in queue.
        while(ingest.line_number != *x->queued_lines){
            std::cout<<"Read order waiting...\n"; 
            pthread_cond_wait(&queue_cond, &writer_lock);
        }
        x->writefile.append(ingest);
        (*x->queued_lines)++;
        pthread_cond_broadcast(&queue_cond);
        pthread_mutex_unlock(&writer_lock);
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);
    while(pthread_mutex_trylock(&writer_lock)) {
        sleep(1);
    }
    x->writefile.set_eof(true);
    pthread_mutex_unlock(&writer_lock);
    pthread_exit(NULL);
}

void reader::run(int num_threads) {
    read_thread_params params = {this->in, &this->read_lines, this->thewriter, &this->queued_lines};
    pthread_t threads[100];
    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], NULL, read_thread, &params);
    }
    for( int i = 0; i < num_threads; i++ ) {
        pthread_join(threads[i], NULL);
    }
}