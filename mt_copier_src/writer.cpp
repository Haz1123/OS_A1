#include "writer.h"
#include <unistd.h>
#include "timer.h"
// Locks access to shared thread counter.
pthread_mutex_t line_count_lock;
// Locks access to the output file.
pthread_mutex_t write_lock;
// Locks access to shared timing info vec.
pthread_mutex_t timing_info_lock;
// Condition trigger aligned to each queue.
// Avoids repeated order checks.
pthread_cond_t write_order_cond[256];

Writer::Writer(const std::string& name, write_queue_t& write_queue, pthread_mutex_t& queue_mutex):
    write_queue(write_queue), 
    queue_mutex(queue_mutex)
 {
    this->out = std::ofstream(name);
    this->eof_reached = false;
    this->current_line = 0;
    this->lines_written = 0;
    this->timing_info = std::vector<write_loop_time_info>();
}

Writer::~Writer(){
    this->out.close();
    delete this->thread_config;
}

void *write_thread(void *write_thread_params) {
    struct write_thread_parameters *params = (struct write_thread_parameters *)write_thread_params;
    bool timer = params->timer_enabled;
    std::vector<write_loop_time_info> timing;
    clock_t tp_queue_mutex_start;
    clock_t tp_queue_mutex_end;
    clock_t tp_line_read_end;
    clock_t tp_order_wait_start;
    clock_t tp_order_wait_end;
    clock_t tp_line_write_done;
    if(timer){tp_queue_mutex_start = clock();};
    pthread_mutex_lock(&params->queue_mutex);
    while(params->write_queue_iter != params->write_queue_iter_end){
        if(timer){tp_queue_mutex_end = clock();}
        file_line line = *params->write_queue_iter;
        params->write_queue_iter++;
        pthread_mutex_unlock(&params->queue_mutex);
        if(timer){tp_line_read_end = clock();}
        if(timer){tp_order_wait_start = clock();}
        pthread_mutex_lock(&write_lock);
        if(timer){ clock();}
        while(params->next_line_num_write != line.line_number) {
            pthread_cond_wait(&write_order_cond[line.line_number & ORDER_ACCESS_BITMASK], &write_lock);
        }
        if(timer){tp_order_wait_end = clock();}
        params->outfile.write(line.line, READ_CHUNK_SIZE);
        params->next_line_num_write++;
        pthread_cond_broadcast(&write_order_cond[(line.line_number + 1) & ORDER_ACCESS_BITMASK]);
        pthread_mutex_unlock(&write_lock);
        if(timer){
            tp_line_write_done = clock();
            timing.insert(timing.end(), {
                tp_queue_mutex_end - tp_queue_mutex_start, 
                tp_line_read_end - tp_queue_mutex_end,
                tp_order_wait_end - tp_order_wait_start,
                tp_line_write_done - tp_order_wait_end,
            });
            tp_queue_mutex_start = clock();
        }
        // Lock ahead of while condition check
        pthread_mutex_lock(&params->queue_mutex);
    }
    pthread_mutex_unlock(&params->queue_mutex);

    if(timer){
        pthread_mutex_lock(&timing_info_lock);
        params->shared_time_info.insert(params->shared_time_info.end(), timing.begin(), timing.end());
        pthread_mutex_unlock(&timing_info_lock);
    }
    pthread_exit(NULL);
}

void Writer::run(int num_threads) {
    this->thread_config = new write_thread_parameters({
        this->out,
        this->queue_mutex,
        this->lines_written,
        this->timer_enabled, 
        this->write_queue.begin(), 
        this->write_queue.end(), 
        this->timing_info,
        });
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