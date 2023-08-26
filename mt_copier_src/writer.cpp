#include "writer.h"
#include <unistd.h>
// Locks access to shared queue
pthread_mutex_t queue_lock;
pthread_mutex_t write_lock;
pthread_mutex_t eof_reached_lock;
pthread_cond_t order_condition[MAX_SUPPORTED_THREADS];
pthread_cond_t write_happened_cond;

Writer::Writer(const std::string& name) {
    this->out = std::ofstream(name);
    this->queue = std::deque<file_line>();
    this->eof_reached = false;
    this->current_line = 0;
}

Writer::~Writer(){
    this->out.close();
    delete this->thread_config;
}

void *write_thread(void *write_thread_params) {
    struct write_thread_parameters *params = (struct write_thread_parameters *)write_thread_params;

    pthread_mutex_lock(&queue_lock);
    while( params->eof_reached == false || params->write_queue.size() > 0 ) {
        // Could be rewritten as a do-while. Left as is for clarity.
        while( params->write_queue.size() > 0 ) {
            std::string line_str = params->write_queue.front().line;
            //int line = params->write_queue.front().line_number;
            params->write_queue.pop_front();
            pthread_mutex_unlock(&queue_lock);

            pthread_mutex_lock(&write_lock);
            // Check if the current line is actually the next line to write
            //while(params->current_line != line) {
                // Wait until this line can be written.
                //pthread_cond_wait(&order_condition[line % params->num_threads], &write_lock);
            //}
            //params->outfile << line_str << "\n";
            params->current_line++;
            //pthread_cond_signal(&order_condition[(line + 1) % params->num_threads]);
            pthread_mutex_unlock(&write_lock);

            pthread_mutex_lock(&queue_lock);
        }
        // Wait for more lines in queue or eof before we eval top level while condition. 
        if(params->eof_reached == false ) {
            pthread_cond_wait(&write_happened_cond, &queue_lock);
        }
    }
    pthread_mutex_unlock(&queue_lock);
    pthread_exit(NULL);
}

void Writer::run(int num_threads) {
    this->thread_config = new write_thread_parameters({this->queue, this->out, this->eof_reached, this->current_line, num_threads});
    for(int i = 0; i < num_threads; i++){
        pthread_create(&this->threads[i], NULL, write_thread, this->thread_config);
    }
}

void Writer::join_threads(int num_threads) {
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(this->threads[i], NULL);
    }
}

void Writer::append(const file_line line) {
    pthread_mutex_lock(&queue_lock);
    this->queue.emplace_back(line);
    pthread_mutex_unlock(&queue_lock);
    pthread_cond_broadcast(&write_happened_cond);
}

void Writer::read_finished() {
    pthread_mutex_lock(&eof_reached_lock);
    this->eof_reached = true;
    pthread_mutex_unlock(&eof_reached_lock);
    pthread_cond_broadcast(&write_happened_cond);
}