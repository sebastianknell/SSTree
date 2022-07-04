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

#define DIM 2
#define M 3
#define m 2

using namespace std;

using Point = vector<double>;

static bool areEqualPoints(Point p1, Point p2) {
    // cout << p1[0] << " " << p1[1] << "-" << p2[0] << " " << p2[1] << endl;
    for (int i = 0; i < DIM; i++) {
        if (fabs(p1[i] - p2[i]) > 20.0) {
            return false;
        }
    }
    return true;
}

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

struct Entry;

//struct Node;

//void updateBoundingEnvelope(Node* node);

struct Node {
    bool isLeaf;
    Circle circle;
    vector<Node*> childs; // valido si no es hoja
    vector<Point> points; // valido si es hoja
    explicit Node(bool isLeaf): isLeaf(isLeaf) {};

    Node(){}

    Node(bool isLeaf, vector<Point> pnts) {
        this->isLeaf = isLeaf;
        points = pnts;
    }

    Node(bool isLeaf, vector<Node*> chld) {
        this->isLeaf = isLeaf;
        childs = chld;
        // updateBoundingEnvelope(this);
    }

    ~Node();
    void mergeChildren(Node*, Node*);
    void addEntry(Entry*);
    void deleteEntry(const Entry*);
    vector<Node*> siblingsToBorrowFrom(Node*);

    Entry* getClosestCentroidTo(Node*);
    Node* findSiblingToMergeTo(Node* n);
};

struct Entry {
    bool value; // 0 = Point, 1 = Node
    Node* node;
    Point* point;

    Entry() {
        node = new Node;
        point = new Point;
    };

    Entry(Node* n) {
        node = n;
        value = 1;
    }

    Entry(Point* p) {
        point = p;
        value = 0;
    }
};

class SSTree {
    Node* root;
public:
    explicit SSTree(): root(nullptr) {};
    ~SSTree() { delete root; };
    void insert(Point&);
    void remove(Point&);
    void show(cv::InputOutputArray&);
};


#endif //SSTREE_SSTREE_H