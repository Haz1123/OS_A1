/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <filesystem>
/* global variables if needed go here */
int main(int argc, char** argv) {
    if(argc <= 2) {
        std::cout << "Program usage: copier.exe [source] [destination]";
        exit(1);
    }
    std::string infile = argv[1];
    std::string outfile = argv[2];

    if( !std::filesystem::exists(infile) ) {
        std::cout << "File:" << infile << " doesn't exist.";
    }

    writer* write = new writer(outfile);
    reader* read = new reader(infile, *write);

    read->run();
    write->run();
    
    return EXIT_SUCCESS;
}

