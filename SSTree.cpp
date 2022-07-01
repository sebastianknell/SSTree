//
// Created by Sebastian Knell on 28/06/22.
//

#include "SSTree.h"

const int order = DIM;
const int m = ceil(order/2);

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

static double getVarianceInRange(vector<Point> points, int direction, int first, int last) {
    double mean = 0;
    for (int i = first; i <= last; i++) mean += points[i][direction];
    mean /= points.size();
    double variance = 0;
    for (int i = first; i <= last; i++) variance += pow(points[i][direction] - mean, 2);
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

static void sortEntriesByCoordinate(Node* node, int coordIndex) {
    static auto lessThanLeaf = [coordIndex](Point a, Point b) {
        return a[coordIndex] < b[coordIndex];
    };
    static auto lessThanInternal = [coordIndex](Node* a, Node* b) {
        return a->circle.center[coordIndex] < b->circle.center[coordIndex];
    };

    if(node->isLeaf) {
        std::sort(node->points.begin(), node->points.end(), lessThanLeaf);
    }
    else{
        std::sort(node->childs.begin(), node->childs.end(), lessThanInternal);
    }
}

int minVarianceSplit(vector<Point> points, int coordIndex){
    double minVariance = INT_MAX;
    int splitIndex = m;

    for(int i = m; i < points.size()-m ; i++){
        double variance1 = getVarianceInRange(points, coordIndex, 0, i-1);
        double variance2 = getVarianceInRange(points, coordIndex, i, points.size()-1);
        if(variance1 + variance2 < minVariance) {
            minVariance = variance1 + variance2;
            splitIndex = i;
        }
    }
    return splitIndex;
}

static int findSplitIndex(Node* node) {
    int coordinateIndex = getMaxVarianceDirection(node);
    sortEntriesByCoordinate(node, coordinateIndex);
    return minVarianceSplit(getCentroids(node), coordinateIndex);
}

void updateBoundingEnvelope(Node* node) {
    auto points = getCentroids(node);
    for (int i = 0 ; i < DIM ; i++) {
        node->circle.center[i] = getMean(points, i);
        node->circle.radius = getMaxDistance(node);
    }
}

// Not used
Node* searchParentLeaf(Node* node, Point target){
    if (node->isLeaf) return node;
    else {
        auto child = findClosestChild(node, target);
        return searchParentLeaf(child, target);
    }
}

bool checkPoint(Node* node, Point point){
    bool found = false;
    for(const auto &p : node->points){
        if (p == point) found = true;
    }
    return found;
}

static pair<Node*,Node*> splitNode(Node* node) {
    auto splitIndex = findSplitIndex(node);
    Node* newNode1 = nullptr;
    Node* newNode2 = nullptr;
    if (node->isLeaf) {
        newNode1 = new Node(true);
        newNode1->points = vector<Point>(node->points.begin(), node->points.begin()+splitIndex-1);
        newNode2 = new Node(true);
        newNode2->points = vector<Point>(node->points.begin()+splitIndex, node->points.end());
    }
    else {
        newNode1 = new Node(true);
        newNode1->childs = vector<Node*>(node->childs.begin(), node->childs.begin()+splitIndex-1);
        newNode2 = new Node(true);
        newNode2->childs = vector<Node*>(node->childs.begin(), node->childs.begin()+splitIndex-1);
    }
    return {newNode1, newNode2};
}

static pair<Node*,Node*> recursiveInsert(Node* node, Point point){
    pair<Node*,Node*> children;
    children.first = nullptr;
    children.second = nullptr;

    if (node->isLeaf) {
        if (checkPoint(node, point)) return children;
        node->points.push_back(point);
        updateBoundingEnvelope(node);

        if (node->points.size() <= DIM) return children;
    }

    else {
        auto closestChild = findClosestChild(node, point);
        children = recursiveInsert(closestChild, point);

        if (children.first == nullptr) {
            updateBoundingEnvelope(node);
            return children;
        }
        else {
            auto iter = std::find_if(node->childs.begin(), node->childs.end(), [closestChild](Node* n) { return n == closestChild; });
            node->childs.erase(iter);
            node->childs.push_back(children.first);
            node->childs.push_back(children.second);
            updateBoundingEnvelope(node);
            if (node->childs.size() <= DIM) {
                children.first = nullptr;
                children.second = nullptr;
                return children;
            }
        }
    }
    return splitNode(node);
}

void SSTree::insert(Point &point) {
    pair<Node*,Node*> newChildren = recursiveInsert(root, point);
    if(newChildren.first != nullptr) {
        root = new Node(false);
        root->childs.push_back(newChildren.first);
        root->childs.push_back(newChildren.second);
    }
}

void SSTree::remove(Point &point) {

}
