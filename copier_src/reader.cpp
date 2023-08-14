/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>
#include <pthread.h>
#include <unistd.h>

const int MAX_POSSIBLE_THREADS = 100;

pthread_mutex_t readfile_lock;
pthread_mutex_t writer_lock;

reader::reader(const std::string& name, writer& mywriter)
    : thewriter(mywriter) {
    this->in.open(name);
}

reader::~reader() {
    this->in.close();
}

struct read_thread_params {
    std::ifstream& infile;
    writer& writefile;
};

void *read_thread(void *read_thread_params) {
    struct read_thread_params *x = (struct read_thread_params *)read_thread_params;
    while(pthread_mutex_trylock(&readfile_lock) != 0){
        sleep(1);
    }
    while(!x->infile.eof()){
        std::string ingest;
        std::getline(x->infile, ingest);
        pthread_mutex_unlock(&readfile_lock);
        while(pthread_mutex_trylock(&writer_lock) != 0 ) {
            sleep(1);
        }
        x->writefile.append(ingest);
        pthread_mutex_unlock(&writer_lock);
        while(pthread_mutex_trylock(&readfile_lock) != 0 ) {
            sleep(1);
        };
    }
    pthread_mutex_unlock(&readfile_lock);
    pthread_exit(NULL);
}

void reader::run(int num_threads) {
    struct read_thread_params params = {this->in, this->thewriter};
    pthread_t threads[100];
    for(int i = 0; i < num_threads; i++){
        
        pthread_create(&threads[i], NULL, read_thread, &params);
    }
    for( int i = 0; i < num_threads; i++ ) {
        pthread_join(threads[i], NULL);
    }
}

