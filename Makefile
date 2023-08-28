
##
 # startup code provided by Paul Miller for COSC1114 - Operating Systems
 # Principles
 ##

# provide make targets here to build the two programs 
all: copier mtcopier

copier: clean
	cp ./timer_src/timer.cpp ./copier_src
	cp ./timer_src/timer.h ./copier_src
	g++ -Wall -Werror -std=c++2a -pthread -pedantic -g -o ./copier.exe ./copier_src/*
	rm -f ./copier_src/timer*

mtcopier: clean
	cp ./timer_src/timer.cpp ./mt_copier_src
	cp ./timer_src/timer.h ./mt_copier_src
	g++ -Wall -Werror -std=c++2a -pthread -pedantic -g -o ./mtcopier.exe ./mt_copier_src/*
	rm -f ./mt_copier_src/timer*

clean:
	rm -f *.o
	rm -f ./copier_src/timer*
	rm -f ./mt_copier_src/timer*