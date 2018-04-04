Welcome to our Distributed Shell! 

The directory is organized in the following manner:

There are two subdirectories, client and server. 
Each has the code and header file to run the corresponding program
(as well as an individual Makefile). 

There is also a master Makefile to compile everything. 

To get started, navigate to the parent directory and enter the command:

> make

To clean up,
> make clean

** Usage **
First, start up the server. To run the server:
> ./server/server

Hints on usage can be found using the -h option
> ./server/server -h

Similarly, the client may be run with the following command:
> ./client/dsh -h

We hard-coded a user-name for the benefit of the grading TAs so that they can run our program
easily, but we initially tested successfully using the syscall that finds our own usernames.


**Acknowledgments**
For the network connections portion of this assignment, 
we got a lot of help from this networks guide:

http://beej.us/guide/bgnet/html/single/bgnet.html
