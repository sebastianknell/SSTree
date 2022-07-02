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

#define DIM 2
#define M 3
#define m 2

using namespace std;
using Point = vector<double>;

extern int radius;
static cv::Scalar colors[] = {
        {106, 100, 208},
        {233, 145, 198},
        {255, 221, 137},
        {255, 170, 130},
        {141, 232, 195},
        {107, 203, 255}
};

struct Circle {Point center; double radius; Circle(): center(DIM), radius(0) {}};

struct Node {
    bool isLeaf;
    Circle circle;
    vector<Node*> childs; // valido si no es hoja
    vector<Point> points; // valido si es hoja
    explicit Node(bool isLeaf): isLeaf(isLeaf) {};
    ~Node();
};

class SSTree {
    Node* root;
    int order;
public:
    explicit SSTree(int order = 3): order(order), root(nullptr) {};
    ~SSTree() { delete root; };
    void insert(Point&);
    void remove(Point&);
    void show(cv::InputOutputArray&);
};


#endif //SSTREE_SSTREE_H
