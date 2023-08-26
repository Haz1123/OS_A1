#include <time.h>
#include <string>

#ifndef TIMER_H
#define TIMER_H

class ProgramTimer {
    public:
        virtual ~ProgramTimer(){};
        // Return current time
        virtual clock_t get_time() = 0;
        // Calculate and print results
        virtual void print_results(
            clock_t start_time,
            clock_t init_finished, 
            clock_t read_finished, 
            clock_t write_finished, 
            clock_t cleanup_finished
        ) = 0;
};

class DisabledProgramTimer: public ProgramTimer {
    public:
        DisabledProgramTimer(){};
        ~DisabledProgramTimer(){};    
        clock_t get_time();
        void print_results(
            clock_t start_time,
            clock_t init_finished, 
            clock_t read_finished, 
            clock_t write_finished, 
            clock_t cleanup_finished
        );

};

class EnabledProgramTimer: public ProgramTimer {
    public:
        EnabledProgramTimer(){};
        ~EnabledProgramTimer(){};
        clock_t get_time();
        void print_results(
            clock_t start_time,
            clock_t init_finished, 
            clock_t read_finished, 
            clock_t write_finished, 
            clock_t cleanup_finished
        );
    private:
        static clock_t time_difference(clock_t lhs, clock_t rhs);
        static std::string format_time_difference(clock_t difference);
};

#endif  //!TIMER_H
