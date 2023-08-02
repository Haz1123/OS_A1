
##
 # startup code provided by Paul Miller for COSC1114 - Operating Systems
 # Principles
 ##

# provide make targets here to build the two programs 
copier:
	g++ -Wall -Werror -std=c++20 -pedantic -g -o ./copier_build/copier.exe ./copier_src/*
