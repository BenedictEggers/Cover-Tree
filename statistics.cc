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
vector<CoverTreePoint> *parse_points(ifstream& f);

// Returns whether the passed character is "okay", i.e. whether it represents
// the start of a number we should process instead of throwing away
static bool keepIterating(char c);

// Takes in a vector of CoverTreePoints, and returns a double that is greater
// than or equal to the maximum distance between any two of the points.
static double getMaxDist(vector<CoverTreePoint> *vec);

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

	double maxDist = getMaxDist(vec);

	cout << "MaxDist: " << maxDist << endl;

	CoverTree<CoverTreePoint> *tree = new CoverTree<CoverTreePoint>(maxDist, *vec);

	// Now process the data somehow

}

vector<CoverTreePoint> *parse_points(ifstream& f) {
	vector<CoverTreePoint> *p = new vector<CoverTreePoint>;  // to return
	vector<double> point;  // to point to the point before we push it on the vector
	double d;  // to hold the new coordinate
	string line;  // to hold the line we just read
	size_t parsed;  // to hold how far in the string we parsed

	// Now read until we're done
	while(!f.eof()) {

		// get the next line
		getline(f, line);

		// make "point" empty so it's actually a new point
		point = vector<double>();

		// Iterate through all the things in the string (arbitrary sequences of
		// characters with some doubles thrown in)
		while (!line.empty()) {
			while (keepIterating(line.at(0))) {
				// cut off the first character until it's something we want
				line = line.substr(1);
			}

			// we're possibly at the end of the file
			if (line.at(0) != '\n' && line.at(0) != EOF) {
				// Now we've cut off the crap at the front, and can actually get a double
				d = std::stod(line, &parsed);

				// And move forward in our string
				line = line.substr(parsed);

				// And stick in on our vector
				point.push_back(d);
			}
		}

		// Now put our beautiful new point onto our points vector
		p->push_back(CoverTreePoint(point, 'a'));

	}
	
	return p;
}

static bool keepIterating(char c) {
	return c != '0' && c != '1' && c != '2' && c != '3' 
  		 && c != '4' && c != '5' && c != '6' && c != '7' 
  		 && c != '8' && c != '9' && c != '\n' && c != EOF && c != '-';
}

static double getMaxDist(vector<CoverTreePoint> *vec) {
	CoverTreePoint zeroPoint(vector<double>(), 'a');
	double max = 0;
	for (uint i = 0; i < vec->size(); i++) {
		double dist = (*vec)[i].distance(zeroPoint);
		if (max < dist) {
			max = dist;
		}
	}
	return 2 * max;
}