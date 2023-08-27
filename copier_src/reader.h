/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "writer.h"
#include <fstream>
#include <iostream>
#include <string>

#ifndef READER
#define READER
class MyReader {
   public:
    /* create a reader that reads each line of the file and appends it to the
     * writer's queue
     */
    MyReader(const std::string& name, Writer& mywriter);
    ~MyReader();
    /* perform the reading from the file */
    void run(bool timer_enabled);

   private:
    std::ifstream in;
    Writer& thewriter;
};
#endif
