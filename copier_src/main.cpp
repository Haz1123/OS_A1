/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
/* global variables if needed go here */
int main(int argc, char** argv) {
    // TODO: Check command line args
    // TODO: Set command line args

    std::string infile = "target.in";
    std::string outfile = "target.out";
    
    writer* write = new writer(outfile);
    reader* read = new reader(infile, *write);

    read->run();
    write->run();
    
    return EXIT_SUCCESS;
}

