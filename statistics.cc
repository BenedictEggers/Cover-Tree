// Author: Ben Eggers (beggerss@cs.washington.edu)
// GNU GPL'd

// This code generates a cover tree from the dataset passed in as a command-line
// parameter, and gathers statistics about the structure of the tree. Or at least,
// that's what it WILL do...

#include <vector>
#include <iostream>

#include "./Cover_Tree.h"
#include "./Cover_Tree_Point.h"


using namespace std;

// Reads in the file, and creates Cover_Tree_Points from its contents. The file 
// must be formatted as follows:
//  - Lines beginning with a "#" will be ignored
//  - One point per line
//  - Points line in n-dimensional euclidean space
//  - Lines must be numbers (floating-point or integer) delimited by any non-number
//    and non-decimal characters
//  - The nth number of each line defines that point's position on the nth axis
//  - If some lines in the same file have more numbers than others, all others 
//    will be padded with 0's so that each point lies in the same-dimensional space
//
// Returns a vector<Cover_Tree_Point> representing all the points in the file.
vector<Cover_Tree_Point> readFile(FILE *f);

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Put in a data file! Noob.";
		exit(1);
	}
}

vector<Cover_Tree_Point>& readFile(FILE *f) {
	return new vector<Cover_Tree_Point>();
}