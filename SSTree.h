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
#include <opencv2/opencv.hpp>

using namespace std;
//using Point = cv::Point;
using Point = struct {int x; int y;};
using Circle = struct {Point center; int radius;};

struct Node {
    bool isLeaf;
    Circle circle;
    int minRadius;
    vector<Circle> regions; // regiones o bounding circles si es hoja
    vector<Node*> childs;
    vector<Point> data; // valido si es hoja
    explicit Node(bool isLeaf): isLeaf(isLeaf) {};
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
