//
// Created by Sebastian Knell on 28/06/22.
//

#ifndef SSTREE_SSTREE_H
#define SSTREE_SSTREE_H

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <random>
#include <cfloat>
#include <opencv2/opencv.hpp>

#ifndef DIM
#define DIM 2
#endif

using namespace std;
//using Point = cv::Point;
using Point = vector<double>;
using Circle = struct {Point center; double radius;};

struct Node {
    bool isLeaf;
    Circle circle;
    int minRadius;
    vector<Node*> childs; // valido si no es hoja
    vector<Point> points; // valido si es hoja
    explicit Node(bool isLeaf): isLeaf(isLeaf) {};
    void updateBoundingEnvelope();
    pair<Node*,Node*> split();
    int findSplitIndex();
    void sortEntriesByCoordinate(int coordIndex);

    ~Node();
};

class SSTree {
    Node* root;
    int order;
public:
    explicit SSTree(int order = 3): order(order), root(nullptr) {};
    ~SSTree() { delete root; };
    void insert(Point);
    void remove(Point);
};


#endif //SSTREE_SSTREE_H
