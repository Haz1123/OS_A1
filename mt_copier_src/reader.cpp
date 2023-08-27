/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t read_lock;
pthread_mutex_t shared_time_info_mutex;

MyReader::MyReader(const std::string& name, Writer& mywriter, write_queue_t& write_queue, pthread_mutex_t& queue_lock)
    : thewriter(mywriter), 
    write_queue(write_queue), 
    queue_mutex(queue_lock)
     {
    this->in = std::ifstream(name);
    this->read_lines= 0;
    this->queued_lines = 0;
    this->general_time_info = std::vector<read_general_time_info>();
    this->loop_time_info = std::vector<read_loop_time_info>();
}

MyReader::~MyReader() {
    this->in.close();
    delete this->thread_parameters;
}

void *read_thread(void *read_thread_params) {
    struct read_thread_params *params = (struct read_thread_params *)read_thread_params;
    bool timer = params->timer;
    clock_t tp_read_mutex_start;
    clock_t tp_read_mutex_end;
    clock_t tp_line_read_end;
    clock_t tp_local_queue_insert_start;
    clock_t tp_local_queue_insert_end;
    clock_t tp_queue_mutex_wait_start;
    clock_t tp_queue_mutex_wait_end;
    clock_t tp_queue_merge_end;
    std::vector<read_loop_time_info> local_loop_time_info;
    
    file_line ingest = file_line();
    write_queue_t local_queue;
    if(timer){tp_read_mutex_start = clock();};
    pthread_mutex_lock(&read_lock);
    while(!params->infile.eof()){
        if(timer){tp_read_mutex_end = clock();};
        params->infile.read(ingest.line, READ_CHUNK_SIZE);
        ingest.line_number = params->finished_read_lines;
        params->finished_read_lines++;
        pthread_mutex_unlock(&read_lock);
        if(timer){tp_line_read_end = clock();};
        if(timer){tp_local_queue_insert_start = clock();};
        local_queue.insert(ingest);
        if(timer){tp_local_queue_insert_end = clock();};
        if(timer){
            local_loop_time_info.insert(local_loop_time_info.end(), {
                tp_read_mutex_end - tp_read_mutex_start,
                tp_line_read_end - tp_read_mutex_end,
                tp_local_queue_insert_end - tp_local_queue_insert_start,
            });
            tp_read_mutex_start = clock();
        }
        ingest = file_line();
        pthread_mutex_lock(&read_lock);
    }
    pthread_mutex_unlock(&read_lock);
    if(timer){tp_queue_mutex_wait_start = clock();};
    pthread_mutex_lock(&params->queue_mutex);
    if(timer){ tp_queue_mutex_wait_end = clock();};
    params->write_queue.merge(local_queue);
    pthread_mutex_unlock(&params->queue_mutex);
    if(timer){tp_queue_merge_end= clock();};

    if(timer){
        pthread_mutex_lock(&shared_time_info_mutex);
        params->loop_time_info.insert(params->loop_time_info.end(), local_loop_time_info.begin(), local_loop_time_info.end());
        params->general_time_info.insert(params->general_time_info.end(), {
            tp_queue_mutex_wait_end - tp_queue_mutex_wait_start,
            tp_queue_merge_end - tp_queue_mutex_wait_end,
        });
        pthread_mutex_unlock(&shared_time_info_mutex);
    }

    pthread_exit(NULL);
}

void MyReader::run(int num_threads) {
    this->thread_parameters = new read_thread_params(
        {
            this->in, 
            this->write_queue, 
            this->queue_mutex,
            this->read_lines, 
            this->queued_lines, 
            this->timer,
            this->loop_time_info, 
            this->general_time_info,
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