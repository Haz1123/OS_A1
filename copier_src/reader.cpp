/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "timer.h"
#include <vector>
#include <functional>

MyReader::MyReader(const std::string& name, Writer& mywriter)
    : thewriter(mywriter) {
    this->in.open(name);
    this->general_time_info = std::vector<read_general_time_info>();
    this->loop_time_info = std::vector<read_loop_time_info>();
}

MyReader::~MyReader() {
    this->in.close();
}

void MyReader::run() {
    clock_t line_read_start;
    clock_t line_read_end;
    clock_t insert_end;
    if(this->timer_enabled){line_read_start = clock();}
    while(!this->in.eof()){
        file_line ingest;
        this->in.read(ingest.line, READ_CHUNK_SIZE);
        if(this->timer_enabled){line_read_end = clock();}
        this->thewriter.append(ingest);
        if(this->timer_enabled){
            insert_end = clock();
            this->loop_time_info.emplace_back(read_loop_time_info({
                clock(),
                line_read_end - line_read_start,
                insert_end - line_read_end
            }));
            line_read_start = clock();
        }
    }

}
