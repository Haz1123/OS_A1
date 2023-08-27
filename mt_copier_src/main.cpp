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

queue_slot_mutexs_t queue_slot_mutexs;
queue_wait_conds_t queue_wait_conds;

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

    ProgramTimer* timer;
    if(argc == 5 && std::string(argv[4]) != "-t") {
        std::cout << PROGRAM_USAGE;
        exit(1);
    } else if (argc == 5 && std::string(argv[4]) == "-t") {
        timer = new EnabledProgramTimer();
    } else {
        timer = new DisabledProgramTimer();
    }

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    const clock_t start_time = timer->get_time();
    
    write_queue_t write_queue;

    Writer* write = new Writer(outfile, write_queue, queue_slot_mutexs, queue_wait_conds);
    MyReader* read = new MyReader(infile, *write, write_queue, queue_slot_mutexs, queue_wait_conds);


    // Some 'initialization' code is still contained in the run function.
    const clock_t init_finish = timer->get_time();

    read->run(num_threads);
    write->run(num_threads);

    read->join_threads(num_threads);
    
    const clock_t read_finish = timer->get_time();

    write->join_threads(num_threads);

    const clock_t write_finish = timer->get_time();

    delete read;
    delete write;

    const clock_t cleanup_finish = timer->get_time();
    
    timer->print_results(start_time, init_finish, read_finish, write_finish, cleanup_finish);
    delete timer;

    return EXIT_SUCCESS;
}

