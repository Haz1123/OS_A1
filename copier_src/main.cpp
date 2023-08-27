/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <filesystem>
#include <time.h>
#include "timer.h"
#include <unistd.h>
// Using these for 'clock time'
#include <chrono>
#include <ctime>

const std::string PROGRAM_USAGE = "Program usage: copier.exe source destination [-t]\n";

/* global variables if needed go here */
int main(int argc, char** argv) {
    if(argc <= 2) {
        std::cout << PROGRAM_USAGE;
        exit(1);
    } else if (argc >= 5) {
        std:: cout << PROGRAM_USAGE;
        exit(1);
    }
    std::string infile = argv[1];
    std::string outfile = argv[2];

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    bool timer_enabled = false;
    if(argc == 4 && argv[3] == std::string("-t")) {
        timer_enabled = true;
    }

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    std::chrono::_V2::system_clock::time_point start = std::chrono::system_clock::now();

    Writer* write = new Writer(outfile);
    MyReader* read = new MyReader(infile, *write);

    write->set_timer_enabled(timer_enabled);
    read->set_timer_enabled(timer_enabled);

    read->run();
    write->run();

    std::chrono::_V2::system_clock::time_point end = std::chrono::system_clock::now();

    if(timer_enabled){
        std::chrono::duration<double> clock_time_taken = end - start;
        std::cout << "Time taken: " << clock_time_taken.count() << " seconds \n";
        // Reader loop info
        std::vector<clock_t> line_read_times;
        std::vector<clock_t> local_queue_insert_times;
        for(read_loop_time_info x : read->loop_time_info){
            line_read_times.emplace_back(x.tp_line_read_time);
            local_queue_insert_times.emplace_back(x.local_queue_insertion_time);
        }
        // Writer loop info
        std::vector<clock_t> lines_read_times;
        std::vector<clock_t> write_times;
        for(write_loop_time_info x : write->timing_info){
            lines_read_times.emplace_back(x.tp_line_read_time);
            write_times.emplace_back(x.tp_write_time);
        }
        // Get times 
        double read_line_read_sum = (double)time_helper::sum_times_real(line_read_times) ;
        double read_local_queue_insert_sum = (double)time_helper::sum_times_real(local_queue_insert_times) ;
        double lines_read_sum = (double)time_helper::sum_times_real(lines_read_times) ;
        double write_sum = (double)time_helper::sum_times_real(write_times) ;
        // Prints to be copied into csv
        std::cout << "Time info: \n";
        std::cout << "single-threaded" << ",";
        std::cout << 1 << ",";
        std::cout << 0 << ",";
        std::cout << read_line_read_sum << ",";
        std::cout << read_local_queue_insert_sum << ",";
        std::cout << 0 << ",";
        std::cout << 0 << ",";
        std::cout << 0 << ",";
        std::cout << lines_read_sum << ",";
        std::cout << 0 << ",";
        std::cout << write_sum << "\n";
    }

    delete read;
    delete write;

    return EXIT_SUCCESS;
}

