#include "writer.h"
#include <unistd.h>
// Locks access to shared thread counter;
pthread_mutex_t line_count_lock;
// Locks access to the output file
pthread_mutex_t write_lock;
// Condition trigger aligned to each queue.
// Helps wake up each thread in order as required.
pthread_cond_t write_order_cond[256];

Writer::Writer(const std::string& name, write_queue_t& write_queue, queue_slot_mutexs_t& queue_slot_mutexs, queue_wait_conds_t& queue_wait_conds):
    write_queue(write_queue), 
    queue_mutexes(queue_slot_mutexs), 
    queue_slot_conds(queue_wait_conds)
 {

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
    std::string output;
    pthread_mutex_lock(&line_count_lock);
    while(params->eof_reached == false || line_num <= params->total_lines){
        line_num = params->next_line_num_read;
        params->next_line_num_read++;
        pthread_mutex_unlock(&line_count_lock);
        pthread_mutex_lock(&params->queue_slot_mutexs[line_num & 255]);
        // Check if line hasn't been read yet.
        while(params->line_queues[line_num & 255]->read == false) {
            std::cout << "Waiting to take off queue:" << line_num << "\n";
            pthread_cond_wait(&params->queue_wait_conds[line_num & 255], &params->queue_slot_mutexs[line_num & 255]);
        }
        file_line line = *params->line_queues[line_num & 255];
        params->line_queues[line_num & 255]->written = true;
        params->line_queues[line_num & 255]->read = false;
        // Wakes up any read threads waiting to put data in.
        pthread_cond_broadcast(&params->queue_wait_conds[line_num & 255]);
        pthread_mutex_unlock(&params->queue_slot_mutexs[line_num & 255]);
        // Check for 'empty' lines.
        if(line.line_number != -1){
            if(line_num != line.line_number){
                std::cout << "SOMETHING GOD READ OUT OF ORDER!!!\n";
                pthread_exit(NULL);
            }
            pthread_mutex_lock(&write_lock);
            while(params->next_line_num_write != line.line_number) {
                //std::cout << "Waiting on write ordering:" << line_num << ":" << params->next_line_num_write << "\n";
                pthread_cond_wait(&write_order_cond[line_num & 255], &write_lock);
                //std::cout << "Cleared write order block for:" << line_num << "\n";
            }
            params->outfile << line.line << "\n";
            params->next_line_num_write++;
            pthread_cond_broadcast(&write_order_cond[(line_num + 1) & 255]);
            pthread_mutex_unlock(&write_lock);
        } else {
            std::cout << "read thingy empty:" << line_num << "\n";
            pthread_mutex_unlock(&params->queue_slot_mutexs[line_num & 255]);
        }
        // Lock ahead of while condition check
        pthread_mutex_lock(&line_count_lock);
    }
    pthread_mutex_unlock(&line_count_lock);
    std::cout << "Write thread finish\n";
    pthread_exit(NULL);
}

void Writer::run(int num_threads) {
    this->thread_config = new write_thread_parameters({this->out, this->write_queue, this->queue_mutexes, this->queue_slot_conds, this->eof_reached, this->current_line, num_threads, this->total_lines, this->written_lines});
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
    // Start at totalLines + 1;
    for (int i = 1; i <= (256 + 1); i++)
    {
        pthread_mutex_lock(&this->queue_mutexes[(total_lines + i) & 255]); 
        while(this->write_queue[(total_lines + i) & 255]->written != true){
            //std::cout<< "cant clear line:" << (total_lines + i) << "\n";
            //std::cout << "Can't clear because write:" << this->write_queue[(total_lines + i) & 255]->line_number << "\n";
            pthread_mutex_unlock(&this->queue_mutexes[(total_lines + i) & 255]);
            sleep(1);
            pthread_mutex_lock(&this->queue_mutexes[(total_lines + i) & 255]); 
        }
        this->write_queue[(total_lines + i) & 255]->line_number = -1;
        this->write_queue[(total_lines + i) & 255]->read = true;
        this->write_queue[(total_lines + i) & 255]->written = false;
        pthread_cond_broadcast(&this->queue_slot_conds[(total_lines + i) & 255]);
        pthread_mutex_unlock(&this->queue_mutexes[(total_lines + i) & 255]);
    }
    std::cout << "finished reading" << "\n";
}