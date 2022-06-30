//
// Created by Sebastian Knell on 28/06/22.
//

#include "SSTree.h"
const int order = 3;

Node::~Node() {
    if (!isLeaf) {
        for (auto &c : childs) delete c;
    }
}

double getMean(vector<Point> points, int dim){
    double sum = 0;
    for(auto point : points){
        sum += point[dim];
    }
    return (sum/points.size());
}

//static double getDistance(Point p1, Point p2){
//    auto x_dif = abs(p1[0] - p2[0]);
//    auto y_dif = abs(p1[1] - p2[1]);
//    return sqrt(pow(x_dif,2) + pow(y_dif,2));
//}

double getDistance(Point a, Point b) {
    double distance = 0.0;
    for (int i = 0; i < DIM; i++)
        distance += pow(a[i] - b[i], 2);
    return sqrt(distance);
}

double getMaxDistance(Node* node){
    double maxDist = 0;
    double tempDist;

    if(node->isLeaf){
        for(const auto& p : node->points){
            tempDist = getDistance(node->circle.center, p);
            if(tempDist > maxDist) maxDist = tempDist;
        }
    }
    else{
        for(auto e : node->childs){
            tempDist = getDistance(node->circle.center, e->circle.center);
            tempDist += node->circle.radius; // No estoy seguro
            if(tempDist > maxDist) maxDist = tempDist;
        }
    }

    return maxDist;
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

void Node::updateBoundingEnvelope() {
    auto points = getCentroids(this);
    for(int i = 0 ; i < DIM ; i++){
        this->circle.center[i] = getMean(points, i);
        this->circle.radius = getMaxDistance(this);
    }
}

// Not used
Node* searchParentLeaf(Node* node, Point target){
    if(node->isLeaf) return node;
    else{
        auto child = findClosestChild(node, target);
        return searchParentLeaf(child, target);
    }
}

bool checkPoint(Node* node, Point point){
    bool found = false;
    for(const auto& p : node->points){
        if(p == point) found = true;
    }
    return found;
}

pair<Node*,Node*> recursiveInsert(Node* node, Point point){
    pair<Node*,Node*> children;
    children.first = nullptr;
    children.second = nullptr;

    if(node->isLeaf){
        if(checkPoint(node,point)) return children;
        node->points.push_back(point);
        node->updateBoundingEnvelope();

        if(node->points.size() <= order) return children;
    }

    else{
        auto closestChild = findClosestChild(node, point);
        children = recursiveInsert(closestChild, point);

        if(children.first == nullptr){
            node->updateBoundingEnvelope();
            return children;
        }
        else{
            // TODO
            // node.childs.remove closestChild
            // node.childs.remove children.first
            // node.childs.remove children.second
            node->updateBoundingEnvelope();
            if(node->childs.size() <= order){
                children.first = nullptr;
                children.second = nullptr;
                return children;
            }
        }
    }

    // TODO
    return node.split();

}

void SSTree::insert(Point point) {
    pair<Node*,Node*> newChildren = recursiveInsert(this->root, point);
    if(newChildren.first != nullptr) {
        this->root = new Node(false);
        this->root->childs.push_back(newChildren.first);
        this->root->childs.push_back(newChildren.second);
    }
}

void SSTree::remove(Point point) {

}
