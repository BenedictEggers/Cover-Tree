#include "Cover_Tree_Point.h"
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

double CoverTreePoint::distance(const CoverTreePoint& p) const {
    const vector<double>& otherVec = p.getVec();
    const vector<double>& longer = _vec.size() > otherVec.size() ? _vec : otherVec;
    const vector<double>& shorter = _vec.size() <= otherVec.size() ? _vec : otherVec;
    double dist = 0;
    int shorterSize = shorter.size();
    int longerSize = longer.size();
    // First, compute the distance in the lower-dimensional space
    for (int i = 0; i < shorterSize; i++) {
        double d = longer[i] - shorter[i];
        dist += d*d;
    }
    // Now add the higher dimensions to the calculation
    for (int i = shorterSize; i < longerSize; i++) {
        double d = longer[i];
        dist += d*d;
    }

    return sqrt(dist);
}

const vector<double>& CoverTreePoint::getVec() const {
    return _vec;
}

char CoverTreePoint::getChar() const {
    return _name;
}

void CoverTreePoint::print() const {
    vector<double>::const_iterator it;
    cout << "point " << _name << ": ";
    for(it = _vec.begin(); it != _vec.end(); it++) {
        cout << *it << " ";
    }
    cout << "\n";
}

bool CoverTreePoint::operator==(const CoverTreePoint& p) const {
    return (_vec == p.getVec() && _name == p.getChar());
}
