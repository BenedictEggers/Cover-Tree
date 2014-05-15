// Author: Ben Eggers (beggerss@cs.washington.edu)
// GNU GPL'd

// This code generates a cover tree from the dataset passed in as a command-line
// parameter, and gathers statistics about the structure of the tree. Or at least,
// that's what it WILL do...

#include "Cover_Tree.h"
#include "Cover_Tree_Point.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Reads in the file, and creates CoverTreePoints from its contents. The file 
// must be formatted as follows:
//  - Lines beginning with a "#" will be ignored
//  - One point per line
//  - Points line in n-dimensional euclidean space
//  - Lines must be numbers (floating-point or integer) delimited by any non-number
//    and non-decimal characters
//  - The nth number of each line defines that point's position on the nth axis
//
// Returns a vector<CoverTreePoint> representing all the points in the file.
vector<CoverTreePoint>& parse_points(ifstream& f);


int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Put in a data file! Noob." << endl;
		exit(1);
	}

	// Let's try to open the file
	ifstream dataFile(argv[1]);
	if (!dataFile.is_open()) {
		// Something went wrong! Crash
		cerr << "Unable to open file! Exiting." << endl;
		dataFile.close();
		exit(1);	
	}

	vector<CoverTreePoint> *vec = parse_points(dataFile);
	dataFile.close();

	// Now process the data somehow

	exit(0);
}

vector<CoverTreePoint> *parse_points(ifstream& f) {
	vector<CoverTreePoint> *p = new vector<CoverTreePoint>;  // to return
	vector<double> point;  // to point to the point before we push it on the vector
	double f;  // to hold the new coordinate
	string line;  // to hold the line we just read

	// Now read until we're done
	while(!f.eof) {
		getline(f, line);  // next line
		while(isOkay(str.at(0))) {
			
		}
	}
	
	return p;
}
