Group:
Jake Adicoff
Ethan Zhou
Chad Carrera

Code:
To run the code, cd into the SSAT folder. Call make. The executable will be a.out and there are now arguments necessary on execution. Drop files you want to test straight into the SSAT folder, and enter the name of the file into terminal when prompted by the code. All other running procedure is self explanatory.

Test files:
Our test files are in the directory "./ssat-test-problems/". There is an option to run all 18 files with all 7 ways to solve the problem, this will probably take a long time - run at your own risk. (this actually has a bug, thinking a memory leak, so it was not used to run our tests.)

Notes:
1. rtimer.c and rtimer.h (used for timing the solver) are not our code. They specify conditions for use and redistribution, all of which we have followed.
2. There is a small piece of code in ssat.cpp that has been adapted from a Stack Overflow thread. The code involves grabbing filenames from a directory, and was used to get the names of all 18 test files. The link to the thread is:  http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c