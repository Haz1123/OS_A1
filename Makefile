
##
 # startup code provided by Paul Miller for COSC1114 - Operating Systems
 # Principles
 ##

# provide make targets here to build the two programs 
copier:
	g++ -Wall -Werror -std=c++2a -pedantic -g -o ./copier.exe ./copier_src/*
