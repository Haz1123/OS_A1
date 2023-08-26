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
    ProgramTimer* timer;
    if(argc == 4 && std::string(argv[3]) != "-t") {
        std::cout << PROGRAM_USAGE;
        exit(1);
    } else if (argc == 4 && std::string(argv[3]) == "-t") {
        timer = new EnabledProgramTimer();
    } else {
        timer = new DisabledProgramTimer();
    }

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    const clock_t start_time = timer->get_time();

    Writer* write = new Writer(outfile);
    MyReader* read = new MyReader(infile, *write);

    const clock_t init_finish = timer->get_time();

    read->run();

    const clock_t read_finish = timer->get_time();

    write->run();

    const clock_t write_finish = timer->get_time();

    delete read;
    delete write;

    const clock_t cleanup_finish = timer->get_time();

    timer->print_results(start_time, init_finish, read_finish, write_finish, cleanup_finish);
    delete timer;

    return EXIT_SUCCESS;
}

