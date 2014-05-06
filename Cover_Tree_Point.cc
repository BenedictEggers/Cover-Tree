#include "Cover_Tree_Point.h"
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

double CoverTreePoint::distance(const CoverTreePoint& p) const {
    const vector<double>& otherVec = p.getVec();
    double dist = 0;
    int lim = otherVec.size();
    for (int i = 0; i < lim; i++) {
        double d = otherVec[i] - _vec[i];
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
