Kyle Schultz

Programmin Assignment 3

Included files: multi-lookup.c, multi-lookup.h, README.txt, Makefile, performance.txt, performance.py, performance.png.

This assignment contains multi-lookup.c and multi-lookup.h.Multi-lookup.c creates a multi-threaded program that gives the 
ip address for given input files. The input files should contain one domain name on each line. The program will print 
the output of the dnslookups to a file named results.txt. The requester threads will also print their thread id and the
number of files they serviced to a file called serviced.txt. The total time that the program run will be shown in the
standard output before the program exits. 

To compile the program, run 'make'. This will execute the command gcc -Wall -Wextra -pthread -o multi-lookup multi-lookup.c.

To run the program use the command ./multi-lookup <# requester> <# resolver> <requester log> <resolver log>
[ <data file> ...] 

To remove the executable run the command 'make clean'.

To check for memory leaks with valgrind run 'valgrind ./multi-lookup <# requester> <# resolver> <requester log> 
<resolver log> [ <data file> ...]'

To plot the performance of this program run the command 'python performance.py multi-lookup > dev/null &'. Run this command
with the performance.py file submitted because it has been edited to work with the input files and my program. 
