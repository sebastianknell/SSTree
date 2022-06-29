//
// Created by Sebastian Knell on 28/06/22.
//

#include "SSTree.h"

Node::~Node() {
    if (!isLeaf) {
        for (auto &c : childs) delete c;
    }
}

double getDistance(Point a, Point b) {
    double distance = 0.0;
    for (int i = 0; i < DIM; i++)
        distance += pow(a[i] - b[i], 2);
    return sqrt(distance);
}

static double getVariance(vector<Point> points, int direction) {
    double mean = 0;
    for (auto &p : points) mean += p[direction];
    mean /= points.size();
    double variance = 0;
    for (auto &p : points) variance += pow(p[direction] - mean, 2);
    return variance / points.size();
}

static vector<Point> getCentroids(Node* node) {
    if (node->isLeaf) return node->points;
    vector<Point> centroids;
    for (auto &c : node->childs)
        centroids.push_back(c->circle.center);
    return centroids;
}

static Node* findClosestChild(Node* node, Point p) {
    assert(!node->isLeaf);
    double min = INT_MAX;
    Node* result = nullptr;
    for (auto &c : node->childs) {
        auto distance = getDistance(c->circle.center, p);
        if (distance < min) {
            min = distance;
            result = c;
        }
    }
    return result;
}

int getMaxVarianceDirection(Node* node) {
    double maxVariance = 0;
    int direction = 0;
    auto centroids = getCentroids(node);
    for (int i = 0; i < DIM; i++) {
        auto variance = getVariance(centroids, i);
        if (variance > maxVariance) maxVariance = variance;
        direction = i;
    }
    return direction;
}

void SSTree::insert(Point point) {

}

void SSTree::remove(Point point) {

}
