#include "writer.h"
#include <unistd.h>
// Locks access to shared thread counter;
pthread_mutex_t line_count_lock;
// Locks access to the output file
pthread_mutex_t write_lock;
// Condition trigger aligned to each queue.
// Helps wake up each thread in order as required.
pthread_cond_t write_order_cond[256];

Writer::Writer(const std::string& name, write_queue_t& write_queue, pthread_mutex_t& queue_mutex):
    write_queue(write_queue), 
    queue_mutex(queue_mutex)
 {

    this->out = std::ofstream(name);

    this->eof_reached = false;
    this->current_line = 0;
    this->lines_written = 0;
}

Writer::~Writer(){
    this->out.close();
    delete this->thread_config;
}

void *write_thread(void *write_thread_params) {
    struct write_thread_parameters *params = (struct write_thread_parameters *)write_thread_params;
    bool timer = params->timer_enabled;
    int line_num = 0;
    clock_t tp_loop_start;
    clock_t tp_queue_lock;
    clock_t tp_line_read;
    clock_t tp_write_lock;
    clock_t tp_line_order;
    clock_t tp_line_write;
    clock_t tp_loop_finish;
    int ti_num_loops = 0;
    std::string output;
    pthread_mutex_lock(&line_count_lock);
    while(params->eof_reached == false || line_num <= params->total_lines){
        line_num = params->next_line_num_read;
        params->next_line_num_read++;
        pthread_mutex_unlock(&line_count_lock);
        ti_num_loops++;
        if(timer){tp_loop_start = clock();}
        pthread_mutex_lock(&params->queue_mutex);
        file_line_ptr line = params->write_queue.front();
        params->write_queue.pop_front();
        if(timer){tp_line_read = clock();}
        pthread_mutex_unlock(&params->queue_mutex);
        if(timer){tp_queue_lock = clock();}
        pthread_mutex_lock(&write_lock);
        if(timer){tp_write_lock = clock();}
        while(params->next_line_num_write != line->line_number) {
            //std::cout << "Thread:"<<line->line_number << "waiting on " << params->next_line_num_write << "\n";
            pthread_cond_wait(&write_order_cond[line->line_number & QUEUE_ACCESS_BITMASK], &write_lock);\
            //std::cout << "Thread:" << line->line_number << "woken up \n";
        }
        if(timer){tp_line_order = clock();}
        //std::cout << line->line << "---" << line->line_number << "\n";
        params->outfile << line->line;
        params->next_line_num_write++;
        pthread_cond_broadcast(&write_order_cond[(line->line_number + 1) & QUEUE_ACCESS_BITMASK]);
        pthread_mutex_unlock(&write_lock);
        if(timer){tp_line_write = clock();}
        delete line;
        if(timer){tp_loop_finish = clock();}
        // Lock ahead of while condition check
        pthread_mutex_lock(&line_count_lock);
        if(!timer){
            std::cout << tp_line_order << tp_line_read << tp_line_write << tp_loop_finish << tp_loop_start << tp_queue_lock << tp_write_lock << ti_num_loops;
        }
    }
    pthread_mutex_unlock(&line_count_lock);
    pthread_exit(NULL);
}

void Writer::run(int num_threads, bool timer_enabled) {
    this->thread_config = new write_thread_parameters({this->out, this->write_queue, this->queue_mutex, this->eof_reached, this->current_line, num_threads, this->total_lines, this->lines_written, timer_enabled});
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


void Writer::read_finished(int total_lines) {
    // Not using a mutex lock as this is only called once.
    // No race conditions are caused here.
    pthread_mutex_lock(&line_count_lock);
    this->eof_reached = true;
    this->total_lines = total_lines;
    pthread_mutex_unlock(&line_count_lock);
    pthread_mutex_lock(&this->queue_mutex);
    // Push enough lines to catch every thread.
    for (int i = 0; i <= MAX_SUPPORTED_THREADS; i++)
    {
        file_line_ptr dummyLine = new file_line({"", total_lines + i});
        this->write_queue.push_front(dummyLine);
    }
    pthread_mutex_unlock(&this->queue_mutex);
}