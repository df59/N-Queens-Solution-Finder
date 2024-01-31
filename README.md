N-Queens Solution Finder,
Dustin Franklin,
df59@uakron.edu

Given a N*N board with the inital position of one queen, finds a possible solution for the arrangement of N queens
such that no queen is currently threatened by a direct attack from another queen. If no solution exists, outputs
"No solution".

To compile run g++ main.cpp.
To run simply run the executable with no arguments.

There must be a csv file in the working directory of the executable called "input.csv" with rigid NxN format of 0 for "no queen" and 1 for "queen." Output is
in the same directory, named "output.csv".

This is a recursive method of the backtracking algorithm, solving quickly when the stack does not overflow. 
Future consideration will be to use internal memory handling rather than keeping track of previous board states
with recursive function calls.
