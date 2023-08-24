/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <filesystem>
#include <pthread.h>

void test() {
    std::ofstream out;
    out.open("test.in");
    for( int i = 0; i < 10000; i ++) {
        out << i << "TESTTESTTESTTESTTEST" << "\n";
    }
    out.close();
}

/* global variables if needed go here */
int main(int argc, char** argv) {
    test();
    if(argc <= 3) {
        std::cout << "Program usage: copier.exe [threads] [source] [destination]";
        exit(1);
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

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    Writer* write = new Writer(outfile);
    MyReader* read = new MyReader(infile, *write);

    read->run(num_threads);
    write->run(num_threads);
    
    read->join_threads(num_threads);
    write->join_threads(num_threads);

    std::cout << "Done.\n";

    delete read;
    delete write;

    return EXIT_SUCCESS;
}

