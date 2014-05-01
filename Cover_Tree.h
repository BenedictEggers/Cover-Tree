/*
 * Copyright (C) 2011 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
 * Written by David Crane <dncrane@gmail.com>
 * Modified by Ben Eggers <ben.eggers36@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _COVER_TREE_H
#define _COVER_TREE_H

#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <float.h>
#include <iostream>

/**
 * Cover Tree. Allows for insertion, removal, and k-nearest-neighbor
 * queries.
 *
 * The user should define double Point::distance(const Point& p) and
 * bool Point::operator==(const Point& p), where
 * p1.distance(p2)==0 doesn't necessarily mean that p1==p2). 
 *
 * For example, a point could consist of a vector and a string
 * name, where their distance measure is simply euclidean distance but to be
 * equal they must have the same name in addition to having distance 0.
 */
template<class Point>
class CoverTree
{
    /**
     * Cover tree node. Consists of arbitrarily many points P, as long as
     * they have distance 0 to each other. Keeps track of its children.
     */
    class CoverTreeNode
    {
    private:
        //_childMap[i] is a vector of the node's children at level i
        std::map<int,std::vector<CoverTreeNode*> > _childMap;
        //_points is all of the points with distance 0 which are not equal.
        std::vector<Point> _points;
    public:
        CoverTreeNode(const Point& p);
        /**
         * Returns the children of the node at level i. Note that this means
         * the children exist in cover set i-1, not level i.
         *
         * Does not include the node itself, though technically every node
         * has itself as a child in a cover tree.
         */
        std::vector<CoverTreeNode*> getChildren(int level) const;
        void addChild(int level, CoverTreeNode* p);
        void removeChild(int level, CoverTreeNode* p);
        void addPoint(const Point& p);
        void removePoint(const Point& p);
        const std::vector<Point>& getPoints() { return _points; }
        double distance(const CoverTreeNode& p) const;
        
        bool isSingle() const;
        bool hasPoint(const Point& p) const;
            
        const Point& getPoint() const;
        
        /**
         * Return every child of the node from any level. This is handy for
         * the destructor.
         */
        std::vector<CoverTreeNode*> getAllChildren() const;
    }; // CoverTreeNode class
 private:
    typedef std::pair<double, CoverTreeNode*> distNodePair;

    CoverTreeNode* _root;
    unsigned int _numNodes;
    int _maxLevel;//base^_maxLevel should be the max distance
                  //between any 2 points
    int _minLevel;//A level beneath which there are no more new nodes.

    std::vector<CoverTreeNode*>
        kNearestNodes(const Point& p, const unsigned int& k) const;
    /**
     * Recursive implementation of the insert algorithm (see paper).
     */
    bool insert_rec(const Point& p,
                    const std::vector<distNodePair>& Qi,
                    const int& level);
    
    /**
     * Finds the node in Q with the minimum distance to p. Returns a
     * pair consisting of this node and the distance.
     */
    distNodePair distance(const Point& p,
                          const std::vector<CoverTreeNode*>& Q);

    
    void remove_rec(const Point& p,
                    std::map<int,std::vector<distNodePair> >& coverSets,
                    int level,  
                    bool& multi);

 public:
    static const double base = 2.0;

    /**
     * Constructs a cover tree which begins with all points in points.
     *
     * maxDist should be the maximum distance that any two points
     * can have between each other. IE p.distance(q) < maxDist for all
     * p,q that you will ever try to insert. The cover tree may be invalid
     * if an inaccurate maxDist is given.
     */
    
    CoverTree(const double& maxDist,
              const std::vector<Point>& points=std::vector<Point>()); 
    ~CoverTree();

    /**
     * Just for testing/debugging. Returns true iff the cover tree satisfies the
     * the covering tree invariants (every node in level i is greater than base^i
     * distance from every other node, and every node in level i is less than
     * or equal to base^i distance from its children). See the cover tree
     * papers for details.
     */
    bool isValidTree() const;

    /**
     * Insert newPoint into the cover tree. If newPoint is already present,
     * (that is, newPoint==p for some p already in the tree), then the tree
     * is unchanged. If p.distance(newPoint)==0.0 but newPoint!=p, then
     * newPoint WILL be inserted and both points may be returned in k-nearest-
     * neighbor searches.
     */
    void insert(const Point& newPoint);

    /**
     * Remove point p from the cover tree. If p is not present in the tree,
     * it will remain unchanged. Otherwise, this will remove exactly one
     * point q from the tree satisfying p==q.
     */
    void remove(const Point& p);

    /**
     * Returns the k nearest points to p in order (the 0th element of the vector
     * is closest to p, 1th is next, etc). It may return greater than k points
     * if there is a tie for the kth place.
     */
    std::vector<Point> kNearestNeighbors(const Point& p, const unsigned int& k) const;

    CoverTreeNode* getRoot() const;

    /**
     * Print the cover tree.
     */
    void print() const;
}; // CoverTree class

#endif // _COVER_TREE_H
 
