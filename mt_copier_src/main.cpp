/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <filesystem>
#include <pthread.h>
#include <time.h>
#include <map>
#include <vector>
#include "timer.h"

const std::string PROGRAM_USAGE = "Program usage: copier.exe n_threads source destination [-t]";

pthread_mutex_t queue_mutex;

/* global variables if needed go here */
int main(int argc, char** argv) {
    if(argc <= 3) {
        std::cout << PROGRAM_USAGE;
        exit(1);
    } else if (argc >=6) {
        std:: cout << PROGRAM_USAGE;
    }

    std::int16_t num_threads;
    try{
        num_threads = std::stoi(argv[1]);
    } catch (std::invalid_argument const& ex) {
        std::cout << "Failed to convert arg 1=" << argv[1] << " to a number.";
        exit(1);
    }
    std::string infile = argv[2];
    std::string outfile = argv[3];
    bool timer_enabled = false;
    if(argc == 5 && argv[4] == std::string("-t")) {
        timer_enabled = true;
    }

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }
    
    write_queue_t write_queue;

    Writer* write = new Writer(outfile, write_queue, queue_mutex);
    MyReader* read = new MyReader(infile, *write, write_queue, queue_mutex);

    write->set_timer_enabled(timer_enabled);
    read->set_timer_enabled(timer_enabled);

    read->run(num_threads);
    // Need to ensure all threads are finished for 'line' ordering.
    read->join_threads(num_threads);

    if(timer_enabled){std::cout << "Read finished.\n";};

    write->run(num_threads);    
    write->join_threads(num_threads);
    if(timer_enabled){std::cout << "Write finished.\n";};

    if(timer_enabled){
        // Reader loop info
        std::vector<clock_t> read_mutex_times;
        std::vector<clock_t> line_read_times;
        std::vector<clock_t> local_queue_insert_times;
        for(read_loop_time_info x : read->loop_time_info){
            read_mutex_times.emplace_back(x.readfile_mutex_time);
            line_read_times.emplace_back(x.tp_line_read_time);
            local_queue_insert_times.emplace_back(x.local_queue_insertion_time);
        }
        // Reader thread info
        std::vector<clock_t> queue_mutex_wait_times;
        std::vector<clock_t> queue_merge_times;
        for(read_general_time_info x : read->general_time_info) {
            queue_mutex_wait_times.emplace_back(x.queue_mutex_wait);
            queue_merge_times.emplace_back(x.queue_merge_time);
        }
        // Writer loop info
        std::vector<clock_t> queue_mutex_times;
        std::vector<clock_t> lines_read_times;
        std::vector<clock_t> write_order_times;
        std::vector<clock_t> write_times;
        for(write_loop_time_info x : write->timing_info){
            queue_mutex_times.emplace_back(x.tp_queue_mutex_time);
            lines_read_times.emplace_back(x.tp_line_read_time);
            write_order_times.emplace_back(x.tp_write_order_time);
            write_times.emplace_back(x.tp_write_time);
        }
        std::cout << read_mutex_times.size() << "\n";
        // Get times 
        double read_queue_mutex_sum = (double)time_helper::sum_times_real(read_mutex_times) ;
        double read_line_read_sum = (double)time_helper::sum_times_real(line_read_times) ;
        double read_local_queue_insert_sum = (double)time_helper::sum_times_real(local_queue_insert_times) ;
        double read_queue_mutex_wait_sum = (double)time_helper::sum_times_real(queue_mutex_wait_times) ;
        double read_queue_merge_sum = (double)time_helper::sum_times_real(queue_merge_times) ;
        double queue_mutex_sum = (double)time_helper::sum_times_real(queue_mutex_times) ;
        double lines_read_sum = (double)time_helper::sum_times_real(lines_read_times) ;
        double write_order_sum = (double)time_helper::sum_times_real(write_order_times) ;
        double write_sum = (double)time_helper::sum_times_real(write_times) ;
        // Prints to be copied into csv
        std::cout << "Time info: \n";
        std::cout << "multi-threaded" << ",";
        std::cout << num_threads << ",";
        std::cout << read_queue_mutex_sum << ",";
        std::cout << read_line_read_sum << ",";
        std::cout << read_local_queue_insert_sum << ",";
        std::cout << read_queue_mutex_wait_sum << ",";
        std::cout << read_queue_merge_sum << ",";
        std::cout << queue_mutex_sum << ",";
        std::cout << lines_read_sum << ",";
        std::cout << write_order_sum << ",";
        std::cout << write_sum << "\n";
    }

    delete read;
    delete write;

    return EXIT_SUCCESS;
}

