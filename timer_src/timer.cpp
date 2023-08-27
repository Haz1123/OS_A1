#include <time.h>
#include <iostream>
#include "timer.h"

const int CLOCK_T_ERR = ((clock_t)-1);

clock_t DisabledProgramTimer::get_time(){
    return CLOCK_T_ERR;
}

void DisabledProgramTimer::print_results(
    clock_t start_time,
    clock_t init_finished, 
    clock_t read_finished, 
    clock_t write_finished, 
    clock_t cleanup_finished
){
    return;
}

clock_t EnabledProgramTimer::get_time(){
    return clock();
}

void EnabledProgramTimer::print_results(
    clock_t start_time,
    clock_t init_finished, 
    clock_t read_finished, 
    clock_t write_finished, 
    clock_t cleanup_finished
){
    std::cout << "start-init_fin:" << format_time_difference(time_difference(start_time, init_finished)) << "\n";
    std::cout << "init_fin-read_fin:" << format_time_difference(time_difference(init_finished, read_finished)) << "\n";
    std::cout << "init_fin-write_fin:" << format_time_difference(time_difference(init_finished, write_finished)) << "\n";
    std::cout << "write_fin-cleanup_fin:" << format_time_difference(time_difference(write_finished, cleanup_finished)) << "\n";
    std::cout << "start-cleanup_fin:" << format_time_difference(time_difference(start_time, cleanup_finished)) << "\n";

}

clock_t EnabledProgramTimer::time_difference(clock_t start, clock_t end) {
    if(start == ((clock_t)-1) || end == ((clock_t)-1)) {
        return CLOCK_T_ERR;
    } else {
        return end-start;
    }
}

std::string EnabledProgramTimer::format_time_difference(clock_t difference) {
    if( difference == CLOCK_T_ERR ) {
        return std::string("ERROR: clock() encountered an error.");
    } else {
        return std::to_string((double)((double)difference / (double)CLOCKS_PER_SEC));
    }
}