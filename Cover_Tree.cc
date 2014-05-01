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

#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <float.h>
#include <iostream>

#include "Cover_Tree.h"

template<class Point>
CoverTree<Point>::CoverTree(const double& maxDist,
                            const std::vector<Point>& points)
{
    _root=NULL;
    _numNodes=0;
    _maxLevel=ceilf(log(maxDist)/log(base));
    _minLevel=_maxLevel-1;
    typename std::vector<Point>::const_iterator it;
    for(it=points.begin(); it!=points.end(); ++it) {
        this->insert(*it);
    }
}

template<class Point>
CoverTree<Point>::~CoverTree()
{
    if(_root==NULL) return;
    //Get all of the root's children (from any level),
    //delete the root, repeat for each of the children
    std::vector<CoverTreeNode*> nodes;
    nodes.push_back(_root);
    while(!nodes.empty()) {
        CoverTreeNode* byeNode = nodes[0];
        nodes.erase(nodes.begin());
        std::vector<CoverTreeNode*> children = byeNode->getAllChildren();
        nodes.insert(nodes.begin(),children.begin(),children.end());
        //std::cout << _numNodes << "\n";
        delete byeNode;
        //_numNodes--;
    }   
}

template<class Point>
std::vector<typename CoverTree<Point>::CoverTreeNode*>
CoverTree<Point>::kNearestNodes(const Point& p, const unsigned int& k) const
{
    if(_root==NULL) return std::vector<CoverTreeNode*>();
    //maxDist is the kth nearest known point to p, and also the farthest
    //point from p in the set minNodes defined below.
    double maxDist = p.distance(_root->getPoint());
    //minNodes stores the k nearest known points to p.
    std::set<distNodePair> minNodes;

    minNodes.insert(make_pair(maxDist,_root));
    std::vector<distNodePair> Qj(1,make_pair(maxDist,_root));
    for(int level = _maxLevel; level>=_minLevel;level--) {
        typename std::vector<distNodePair>::const_iterator it;
        int size = Qj.size();
        for(int i=0; i<size; i++) {
            std::vector<CoverTreeNode*> children =
                Qj[i].second->getChildren(level);
            typename std::vector<CoverTreeNode*>::const_iterator it2;
            for(it2=children.begin(); it2!=children.end(); ++it2) {
                double d = p.distance((*it2)->getPoint());
                if(d < maxDist || minNodes.size() < k) {
                    minNodes.insert(make_pair(d,*it2));
                    //--minNodes.end() gives us an iterator to the greatest
                    //element of minNodes.
                    if(minNodes.size() > k) minNodes.erase(--minNodes.end());
                    maxDist = (--minNodes.end())->first;
                }
                Qj.push_back(make_pair(d,*it2));
            }
        }
        double sep = maxDist + pow(base, level);
        size = Qj.size();
        for(int i=0; i<size; i++) {
            if(Qj[i].first > sep) {
                //quickly removes an element from a vector w/o preserving order.
                Qj[i]=Qj.back();
                Qj.pop_back();
                size--; i--;
            }
        }
    }
    std::vector<CoverTreeNode*> kNN;
    typename std::set<distNodePair>::const_iterator it;
    for(it=minNodes.begin();it!=minNodes.end();++it) {
        kNN.push_back(it->second);
    }
    return kNN;
}
template<class Point>
bool CoverTree<Point>::insert_rec(const Point& p,
                                  const std::vector<distNodePair>& Qi,
                                  const int& level)
{
    std::vector<std::pair<double, CoverTreeNode*> > Qj;
    double sep = pow(base,level);
    double minDist = DBL_MAX;
    std::pair<double,CoverTreeNode*> minQiDist(DBL_MAX,NULL);
    typename  std::vector<std::pair<double, CoverTreeNode*> >::const_iterator it;
    for(it=Qi.begin(); it!=Qi.end(); ++it) {
        if(it->first<minQiDist.first) minQiDist = *it;
        if(it->first<minDist) minDist=it->first;
        if(it->first<=sep) Qj.push_back(*it);
        std::vector<CoverTreeNode*> children = it->second->getChildren(level);
        typename std::vector<CoverTreeNode*>::const_iterator it2;
        for(it2=children.begin();it2!=children.end();++it2) {
            double d = p.distance((*it2)->getPoint());
            if(d<minDist) minDist = d;
            if(d<=sep) {
                Qj.push_back(make_pair(d,*it2));
            }
        }
    }
    //std::cout << "level: " << level << ", sep: " << sep << ", dist: " << minQDist.first << "\n";
    if(minDist > sep) {
        return true;
    } else {
        bool found = insert_rec(p,Qj,level-1);
        //distNodePair minQiDist = distance(p,Qi);
        if(found && minQiDist.first <= sep) {
            if(level-1<_minLevel) _minLevel=level-1;
            minQiDist.second->addChild(level,
                                       new CoverTreeNode(p));
            //std::cout << "parent is ";
            //minQiDist.second->getPoint().print();
            _numNodes++;
            return false;
        } else {
            return found;
        }
    }
}

template<class Point>
void CoverTree<Point>::remove_rec(const Point& p,
                                  std::map<int,std::vector<distNodePair> >& coverSets,
                                  int level,
                                  bool& multi)
{
    std::vector<distNodePair>& Qi = coverSets[level];
    std::vector<distNodePair>& Qj = coverSets[level-1];
    double minDist = DBL_MAX;
    CoverTreeNode* minNode = _root;
    CoverTreeNode* parent = 0;
    double sep = pow(base, level);
    typename std::vector<distNodePair>::const_iterator it;
    //set Qj to be all children q of Qi such that p.distance(q)<=sep
    //and also keep track of the minimum distance from p to a node in Qj
    //note that every node has itself as a child, but the
    //getChildren function only returns non-self-children.
    for(it=Qi.begin();it!=Qi.end();++it) {
        std::vector<CoverTreeNode*> children = it->second->getChildren(level);
        double dist = it->first;
        if(dist<minDist) {
            minDist = dist;
            minNode = it->second;
        }
        if(dist <= sep) {
            Qj.push_back(*it);
        }
        typename std::vector<CoverTreeNode*>::const_iterator it2;
        for(it2=children.begin();it2!=children.end();++it2) {
            dist = p.distance((*it2)->getPoint());
            if(dist<minDist) {
                minDist = dist;
                minNode = *it2;
                if(dist == 0.0) parent = it->second;
            }
            if(dist <= sep) {
                Qj.push_back(make_pair(dist,*it2));
            }
        }
    }
    if(level>_minLevel) remove_rec(p,coverSets,level-1,multi);
    if(minNode->hasPoint(p)) {
        //the multi flag indicates the point we removed is from a
        //node containing multiple points, and we have removed it,
        //so we don't need to do anything else.
        if(multi) return;
        if(!minNode->isSingle()) {
            minNode->removePoint(p);
            multi=true;
            return;
        }
        if(parent!=NULL) parent->removeChild(level, minNode);
        std::vector<CoverTreeNode*> children = minNode->getChildren(level-1);
        std::vector<distNodePair>& Q = coverSets[level-1];
        if(Q.size()==1 && Q[0].second==minNode) {
            Q.pop_back();
        } else {
            for(unsigned int i=0;i<Q.size();i++) {
                if(Q[i].second==minNode) {
                    Q[i]=Q.back();
                    Q.pop_back();
                    break;
                }
            }
        }
        typename std::vector<CoverTreeNode*>::const_iterator it;
        for(it=children.begin();it!=children.end();++it) {
            int i = level-1;
            Point q = (*it)->getPoint();
            double minDQ = DBL_MAX;
            CoverTreeNode* minDQNode;
            double sep = pow(base,i);
            bool br=false;
            while(true) {
                std::vector<distNodePair>&
                    Q = coverSets[i];
                typename std::vector<distNodePair>::const_iterator it2;
                minDQ = DBL_MAX;
                for(it2=Q.begin();it2!=Q.end();++it2) {
                    double d = q.distance(it2->second->getPoint());
                    if(d<minDQ) {
                        minDQ = d;
                        minDQNode = it2->second;
                        if(d <=sep) {
                            br=true;
                            break;
                        }
                    }
                }
                minDQ=DBL_MAX;
                if(br) break;
                Q.push_back(make_pair((*it)->distance(p),*it));
                i++;
                sep = pow(base,i);
            }
            //minDQNode->getPoint().print();
            //std::cout << " is level " << i << " parent of ";
            //(*it)->getPoint().print();
            minDQNode->addChild(i,*it);
        }
        if(parent!=NULL) {
            delete minNode;
            _numNodes--;
        }
    }
}

template<class Point>
std::pair<double, typename CoverTree<Point>::CoverTreeNode*>
CoverTree<Point>::distance(const Point& p,
                           const std::vector<CoverTreeNode*>& Q)
{
    double minDist = DBL_MAX;
    CoverTreeNode* minNode;
    typename std::vector<CoverTreeNode*>::const_iterator it;
    for(it=Q.begin();it!=Q.end();++it) {
        double dist = p.distance((*it)->getPoint());
        if(dist < minDist) {
            minDist = dist;
            minNode = *it;
        }
    }
    return make_pair(minDist,minNode);  
}

template<class Point>
void CoverTree<Point>::insert(const Point& newPoint)
{
    if(_root==NULL) {
        _root = new CoverTreeNode(newPoint);
        _numNodes=1;
        return;
    }
    //TODO: this is pretty inefficient, there may be a better way
    //to check if the node already exists...
    CoverTreeNode* n = kNearestNodes(newPoint,1)[0];
    if(newPoint.distance(n->getPoint())==0.0) {
        n->addPoint(newPoint);
    } else {
        //insert_rec acts under the assumption that there are no nodes with
        //distance 0 to newPoint in the cover tree (the previous lines check it)
        insert_rec(newPoint,
                   std::vector<distNodePair>
                   (1,make_pair(_root->distance(newPoint),_root)),
                   _maxLevel);
    }
}

template<class Point>
void CoverTree<Point>::remove(const Point& p)
{
    //Most of this function's code is for the special case of removing the root
    if(_root==NULL) return;
    bool removingRoot=_root->hasPoint(p);
    if(removingRoot && !_root->isSingle()) {
        _root->removePoint(p);
        return;
    }
    CoverTreeNode* newRoot=NULL;
    if(removingRoot) {
        if(_numNodes==1) {
            //removing the last node...
            delete _root;
            _numNodes--;
            _root=NULL;
            return;
        } else {
            for(int i=_maxLevel;i>_minLevel;i--) {
                if(!(_root->getChildren(i).empty())) {
                    newRoot = _root->getChildren(i).back();
                    _root->removeChild(i,newRoot);
                    break;
                }
            }
        }
    }
    std::map<int, std::vector<distNodePair> > coverSets;
    coverSets[_maxLevel].push_back(make_pair(_root->distance(p),_root));
    if(removingRoot)
        coverSets[_maxLevel].push_back(make_pair(newRoot->distance(p),newRoot));
    bool multi = false;
    remove_rec(p,coverSets,_maxLevel,multi);
    if(removingRoot) {
        delete _root;
        _numNodes--;
        _root=newRoot;
    }
}

template<class Point>
std::vector<Point> CoverTree<Point>::kNearestNeighbors(const Point& p,
                                                       const unsigned int& k) const
{
    if(_root==NULL) return std::vector<Point>();
    std::vector<CoverTreeNode*> v = kNearestNodes(p, k);
    std::vector<Point> kNN;
    typename std::vector<CoverTreeNode*>::const_iterator it;
    for(it=v.begin();it!=v.end();++it) {
        const std::vector<Point>& p = (*it)->getPoints();
        kNN.insert(kNN.end(),p.begin(),p.end());
        if(kNN.size() >= k) break;
    }
    return kNN;
}

template<class Point>
void CoverTree<Point>::print() const
{
    int d = _maxLevel-_minLevel+1;
    std::vector<CoverTreeNode*> Q;
    Q.push_back(_root);
    for(int i=0;i<d;i++) {
        std::cout << "LEVEL " << _maxLevel-i << "\n";
        typename std::vector<CoverTreeNode*>::const_iterator it;
        for(it=Q.begin();it!=Q.end();++it) {
            (*it)->getPoint().print();
            std::vector<CoverTreeNode*>
                children = (*it)->getChildren(_maxLevel-i);
            typename std::vector<CoverTreeNode*>::const_iterator it2;
            for(it2=children.begin();it2!=children.end();++it2) {
                std::cout << "  ";
                (*it2)->getPoint().print();
            }
        }
        std::vector<CoverTreeNode*> newQ;
        for(it=Q.begin();it!=Q.end();++it) {
            std::vector<CoverTreeNode*>
                children = (*it)->getChildren(_maxLevel-i);
            newQ.insert(newQ.end(),children.begin(),children.end());
        }
        Q.insert(Q.end(),newQ.begin(),newQ.end());
        std::cout << "\n\n";
    }
}

template<class Point>
typename CoverTree<Point>::CoverTreeNode* CoverTree<Point>::getRoot() const
{
    return _root;
}

template<class Point>
CoverTree<Point>::CoverTreeNode::CoverTreeNode(const Point& p) {
    _points.push_back(p);
}

template<class Point>
std::vector<typename CoverTree<Point>::CoverTreeNode*>
CoverTree<Point>::CoverTreeNode::getChildren(int level) const
{
    typename std::map<int,std::vector<CoverTreeNode*> >::const_iterator
        it = _childMap.find(level);
    if(it!=_childMap.end()) {
        return it->second;
    }
    return std::vector<CoverTreeNode*>();
}

template<class Point>
void CoverTree<Point>::CoverTreeNode::addChild(int level, CoverTreeNode* p)
{
    _childMap[level].push_back(p);
}

template<class Point>
void CoverTree<Point>::CoverTreeNode::removeChild(int level, CoverTreeNode* p)
{
    std::vector<CoverTreeNode*>& v = _childMap[level];
    for(unsigned int i=0;i<v.size();i++) {
        if(v[i]==p) {
            v[i]=v.back();
            v.pop_back();
            break;
        }
    }
}

template<class Point>
void CoverTree<Point>::CoverTreeNode::addPoint(const Point& p)
{
    if(find(_points.begin(), _points.end(), p) == _points.end())
        _points.push_back(p);
}

template<class Point>
void CoverTree<Point>::CoverTreeNode::removePoint(const Point& p)
{
    typename std::vector<Point>::iterator it =
        find(_points.begin(), _points.end(), p);
    if(it != _points.end())
        _points.erase(it);
}

template<class Point>
double CoverTree<Point>::CoverTreeNode::distance(const CoverTreeNode& p) const
{
    return _points[0].distance(p.getPoint());
}
 
template<class Point>
bool CoverTree<Point>::CoverTreeNode::isSingle() const
{
    return _points.size() == 1;
}

template<class Point>
bool CoverTree<Point>::CoverTreeNode::hasPoint(const Point& p) const
{
    return find(_points.begin(), _points.end(), p) != _points.end();
}

template<class Point>
const Point& CoverTree<Point>::CoverTreeNode::getPoint() const { return _points[0]; }

template<class Point>
std::vector<typename CoverTree<Point>::CoverTreeNode*>
CoverTree<Point>::CoverTreeNode::getAllChildren() const
{
    std::vector<CoverTreeNode*> children;
    typename std::map<int,std::vector<CoverTreeNode*> >::const_iterator it;
    for(it=_childMap.begin();it!=_childMap.end();++it) {
        children.insert(children.end(), it->second.begin(), it->second.end());
    }
    return children;
}

template<class Point>
bool CoverTree<Point>::isValidTree() const {
    if(_numNodes==0)
        return _root==NULL;

    std::vector<CoverTreeNode*> nodes;
    nodes.push_back(_root);
    for(int i=_maxLevel;i>_minLevel;i--) {
        double sep = pow(base,i);
        typename std::vector<CoverTreeNode*>::const_iterator it, it2;
        //verify separation invariant of cover tree: for each level,
        //every point is farther than base^level away
        for(it=nodes.begin(); it!=nodes.end(); ++it) {
            for(it2=nodes.begin(); it2!=nodes.end(); ++it2) {
                double dist=(*it)->distance((*it2)->getPoint());
                if(dist<=sep && dist!=0.0) {
                    std::cout << "Level " << i << " Separation invariant failed.\n";
                    return false;
                }
            }
        }
        std::vector<CoverTreeNode*> allChildren;
        for(it=nodes.begin(); it!=nodes.end(); ++it) {        
            std::vector<CoverTreeNode*> children = (*it)->getChildren(i);
            //verify covering tree invariant: the children of node n at level
            //i are no further than base^i away
            for(it2=children.begin(); it2!=children.end(); ++it2) {
                double dist = (*it2)->distance((*it)->getPoint());
                if(dist>sep) {
                    std::cout << "Level" << i << " covering tree invariant failed.n";
                    return false;
                }
            }
            allChildren.insert
                (allChildren.end(),children.begin(),children.end());
        }
        nodes.insert(nodes.begin(),allChildren.begin(),allChildren.end());
    }
    return true;
}