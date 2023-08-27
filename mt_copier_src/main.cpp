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

    read->run(num_threads);
    read->join_threads(num_threads);

    struct
    {
        bool operator()(const file_line_ptr lhs, const file_line_ptr rhs) const {
            return lhs->line_number < rhs->line_number;
        }
    }
    sort_function;

    std::sort(write_queue.begin(), write_queue.end(), sort_function);

    std::cout << "Read finished.\n"; 

    write->run(num_threads, timer_enabled );    
    write->join_threads(num_threads);

    std::cout << "Write finished.\n";

    delete read;
    delete write;


    return EXIT_SUCCESS;
}

