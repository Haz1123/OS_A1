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

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }


    Writer* write = new Writer(outfile);
    MyReader* read = new MyReader(infile, *write);


    read->run();

    write->run();

    delete read;
    delete write;

    return EXIT_SUCCESS;
}

