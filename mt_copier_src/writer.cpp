#include "writer.h"
#include <unistd.h>
// Locks access to shared thread counter;
pthread_mutex_t line_count_lock;
// Locks access to individual queues.
// Using a fixed length to avoid doing repeated modulo operations
pthread_mutex_t queue_locks[128];
// Locks access to the output file
pthread_mutex_t write_lock;
// Condition trigger aligned to each queue.
// Helps wake up each thread in order as required.
pthread_cond_t order_condition[128];

Writer::Writer(const std::string& name) {
    this->out = std::ofstream(name);
    this->eof_reached = false;
    this->current_line = 0;
    this->written_lines = 0;
}

Writer::~Writer(){
    this->out.close();
    delete this->thread_config;
}

void *write_thread(void *write_thread_params) {
    struct write_thread_parameters *params = (struct write_thread_parameters *)write_thread_params;
    int line_num = 0 ;
    int held_line = 0;
    while( params->eof_reached == false || line_num < params->total_lines ) {
        pthread_mutex_lock(&line_count_lock);
        line_num = params->current_line;
        params->current_line++;
        pthread_mutex_unlock(&line_count_lock);
        pthread_mutex_lock(&queue_locks[line_num & 127]);
        if(params->line_queues[line_num & 127].size() == 0){
            pthread_mutex_unlock(&queue_locks[line_num & 127]);
            //pthread_cond_wait(&write_happened_cond, &queue_locks[line_num & 127]);
        } else {
            held_line = params->line_queues[line_num & 127].front().line_number;
            std::string line = params->line_queues[line_num & 127].front().line;
            params->line_queues[line_num & 127].pop_front();
            pthread_mutex_unlock(&queue_locks[line_num & 127]);
            pthread_mutex_lock(&write_lock);
            while(held_line != params->written_lines){
                pthread_cond_wait(&order_condition[line_num & 127], &write_lock);
            }
            params->outfile << line << "\n";
            params->written_lines++;
            pthread_mutex_unlock(&write_lock);
        }
        pthread_cond_broadcast(&order_condition[(line_num + 1) & 127]);
    }
    pthread_exit(NULL);
}

void Writer::run(int num_threads) {
    this->thread_config = new write_thread_parameters({this->lines, this->out, this->eof_reached, this->current_line, num_threads, this->total_lines, this->written_lines});
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
    pthread_mutex_lock(&queue_locks[line.line_number & 127]);
    this->lines[line.line_number & 127].emplace_back(line);
    pthread_mutex_unlock(&queue_locks[line.line_number & 127]);
}

int Writer::checkQueueInsert(int line_num) {
    int value;
    pthread_mutex_lock(&queue_locks[line_num & 127]);
    if(!this->lines[line_num & 127].empty()){
        value = this->lines[line_num & 127].back().line_number;
    } else {
        value = line_num;
    }
    pthread_mutex_unlock(&queue_locks[line_num & 127]);
    return value;
}

void Writer::read_finished(int total_lines) {
    // Not using a mutex lock as this is only called once.
    // No race conditions are caused here.
    this->eof_reached = true;
    this->total_lines = total_lines;
}