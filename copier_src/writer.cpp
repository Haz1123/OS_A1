#include "writer.h"
#include <unistd.h>

pthread_mutex_t queue_lock;
pthread_mutex_t writefile_lock;

writer::writer(const std::string& name) {
    this->out.open(name);
    this->queue = std::deque<std::string>();
}

writer::~writer() {
    this->out.close();
}

struct write_thread_params {
    std::deque<std::string> *write_queue;
    std::ofstream *outfile;
};

void *write_thread(void *write_thread_params) {
    pthread_detach(pthread_self());
    struct write_thread_params *x = (struct write_thread_params *)write_thread_params;
    std::string output;
    while(pthread_mutex_trylock(&queue_lock) != 0) {
        sleep(1);
    };
    while( x->write_queue->size() > 0 ) {
        output = x->write_queue->front();
        x->write_queue->pop_front();
        pthread_mutex_unlock(&queue_lock);
        while(pthread_mutex_trylock(&writefile_lock) != 0){
            sleep(1);
        }
        x->outfile->write(x->write_queue->front().c_str(), x->write_queue->front().size());
        x->outfile->write("\n", 1);
        pthread_mutex_unlock(&writefile_lock);
        while(pthread_mutex_trylock(&queue_lock) != 0) {
            sleep(1);
        }
    }
    pthread_mutex_unlock(&queue_lock);
    pthread_exit(NULL);
}

void writer::run(int num_threads) {
    struct write_thread_params params = {&this->queue, &this->out};
    pthread_t threads[100] = {0};
    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], NULL, write_thread, &params);
    }
    for( int i = 0; i < num_threads; i++ ) {
        pthread_join(threads[i], NULL);
    }
}

void writer::append(const std::string& line) {
    this->queue.emplace_back(line);
}
