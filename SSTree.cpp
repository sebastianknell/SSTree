//
// Created by Sebastian Knell on 28/06/22.
//

#include "SSTree.h"

int radius = 4;

Node::~Node() {
    if (!isLeaf) {
        for (auto &c : childs) delete c;
    }
}

static double getMean(vector<Point> &points, int dim){
    double sum = 0;
    int i=0;
    cout << "ENTRO GETMEAN, size: " << points.size() << endl;
    for(auto point : points){
        cout << "IT" << i++ << ", size: " << point.size() << " - ";
        sum += point[dim];
    }
    cout << endl;
    cout << "PASO GETMEAN" << endl;
    return (sum/points.size());
}

static double getDistance(Point &a, Point &b) {
    double distance = 0.0;
    for (int i = 0; i < DIM; i++)
        distance += pow(a[i] - b[i], 2);
    return sqrt(distance);
}

double getMaxDistance(Node* node){
    double maxDist = 0;
    double tempDist;
    if(node->isLeaf){
        for(auto &p : node->points){
            tempDist = getDistance(node->circle.center, p);
            if(tempDist > maxDist) maxDist = tempDist;
        }
    }
    else{
        for(auto e : node->childs){
            tempDist = getDistance(node->circle.center, e->circle.center);
            tempDist += e->circle.radius;
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
    if (node->isLeaf)
        cout << "IN GET CENTROIDS IS LEAF - NUMBER OF POINTS: " << node->points.size() << " AND NUMBER OF CHILDS: " << node->childs.size() << endl;
    else
        cout << "IN GET CENTROIDS IS NOT LEAF - NUMBER OF POINTS: " << node->points.size() << " AND NUMBER OF CHILDS: " << node->childs.size() << endl;

    if (node->isLeaf) return node->points;
    vector<Point> centroids;
    for (auto &c : node->childs)
        centroids.push_back(c->circle.center);
    return centroids;
}

static Node* findClosestChild(Node* node, Point &p) {
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

int minVarianceSplit(vector<Point> &points, int coordIndex){
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
    auto centroids = getCentroids(node);
    return minVarianceSplit(centroids, coordinateIndex);
}

void updateBoundingEnvelope(Node* node) {
    auto points = getCentroids(node);
    /*
    for (auto p : points) {
        if (p.size() < 2) {
            cout << "EL PROBLEMA ESTA ACA, SIZE: " << p.size() << endl;
        }

        //cout << p[0] << ", " << p[1] << " - ";
    }
    cout << endl;
    */

    for (int i = 0 ; i < DIM ; i++) {
        node->circle.center[i] = getMean(points, i);
        node->circle.radius = getMaxDistance(node);
        //cout << "Y: " << i << ", center: " << node->circle.center[i] << endl;
    }
}

// Not used
Node* searchParentLeaf(Node* node, Point &target){
    if (node->isLeaf) return node;
    else {
        auto child = findClosestChild(node, target);
        return searchParentLeaf(child, target);
    }
}

bool checkPoint(Node* node, Point &point){
    bool found = false;
    for(const auto &p : node->points){
        if (p == point) found = true;
    }
    return found;
}

static pair<Node*,Node*> splitNode(Node* node) {
    auto splitIndex = findSplitIndex(node);
    Node* newNode1 = new Node(node->isLeaf);
    Node* newNode2 = new Node(node->isLeaf);
    if (node->isLeaf) {
        newNode1->points = vector<Point>(node->points.begin(), node->points.begin()+splitIndex);
        newNode2->points = vector<Point>(node->points.begin()+splitIndex, node->points.end());
    }
    else {
        newNode1->childs = vector<Node*>(node->childs.begin(), node->childs.begin()+splitIndex);
        newNode2->childs = vector<Node*>(node->childs.begin()+splitIndex, node->childs.end());
    }
    updateBoundingEnvelope(newNode1);
    updateBoundingEnvelope(newNode2);
    return {newNode1, newNode2};
}

static pair<Node*,Node*> recursiveInsert(Node* node, Point &point){
    pair<Node*,Node*> children;
    children.first = nullptr;
    children.second = nullptr;

    if (node->isLeaf) {
        if (checkPoint(node, point)) return children;
        node->points.push_back(point);
        updateBoundingEnvelope(node);

        if (node->points.size() <= M) return children;
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
            if (node->childs.size() <= M) {
                children.first = nullptr;
                children.second = nullptr;
                return children;
            }
        }
    }
    return splitNode(node);
}

void SSTree::insert(Point &point) {
    if (!root) {
        root = new Node(true);
        root->points.push_back(point);
    }
    pair<Node*,Node*> newChildren = recursiveInsert(root, point);
    if(newChildren.first != nullptr) {
        root = new Node(false);
        root->childs.push_back(newChildren.first);
        root->childs.push_back(newChildren.second);
    }
}

bool intersectsPoint(Node* node, Point &point) {
    return getDistance(node->circle.center, point) <= node->circle.radius;
}

vector<Node*> Node::siblingsToBorrowFrom(Node* nodeToFix) {
    vector<Node*> siblings;
    for (auto childNode : this->childs) {
        if (childNode != nodeToFix) {
            if (childNode->isLeaf) {
                if (childNode->points.size() > m) siblings.push_back(childNode);
            } else {
                if (childNode->childs.size() > m) siblings.push_back(childNode);
            }
        }
    }
    return siblings;
}

Entry* Node::getClosestCentroidTo(Node* node) {
    Entry* entry = new Entry();

    //cout << "JAJAJJAJAJJAJAJAJAJAJAJAJJAJA" << endl;

    if (node->isLeaf) {
        entry->value = 0;
        for (int i=0; i<this->points.size(); i++) {
            if (i == 0 || getDistance(points[i], node->circle.center) < getDistance(*entry->point, node->circle.center)) {
                entry->point = &this->points[i];
            }
        }

    } else {
        entry->value = 1;
        for (int i=0; i<this->childs.size(); i++) {
            // cout << "i: " << i << ", size: " << this->childs.size() << endl;
            if (i == 0 || getDistance(childs[i]->circle.center, node->circle.center) 
                < getDistance(entry->node->circle.center, node->circle.center)) {
                // cout << "updating" << endl; 
                entry->node = this->childs[i];
            }
        }
        // cout << "FINALIZO EL FOR" << endl;
    }
    //cout << "RETORNA ENTRY" << endl;
    
    return entry;
}

static bool closerThan(Entry* closestEntryInNode, Entry* closestEntry, Node *targetNode) {
    if (closestEntryInNode->value) {  // node
        if (closestEntry == nullptr ||
            getDistance(*closestEntryInNode->point, targetNode->circle.center) <
            getDistance(*closestEntry->point, targetNode->circle.center)
            )
            return true;
    } else {                         // point
        if (closestEntry == nullptr ||
            getDistance(closestEntryInNode->node->circle.center, targetNode->circle.center) <
            getDistance(closestEntry->node->circle.center, targetNode->circle.center)
            )
            return true;
    }
    return false;
}

static pair<Entry*, Node*> findClosestEntryInNodesList(vector<Node*> nodes, Node* targetNode) {
    Entry* closestEntry = nullptr;
    Node* closestNode = nullptr;
    int i=0;
    for (auto node : nodes) {
        Entry* closestEntryInNode = node->getClosestCentroidTo(targetNode);
        cout << "GET CLOSEST CENTROID" << endl;
        if (closerThan(closestEntryInNode, closestEntry, targetNode)) {
            cout << "----------------- VALUE: " << closestEntryInNode->value << endl;
            if (closestEntryInNode->value) {
                if (closestEntryInNode->node->isLeaf) {
                    cout << "ES HOJA - ";
                    cout << "PUNTOS: " << closestEntryInNode->node->points.size() << endl;
                } else {
                    cout << "NO ES HOJA - ";
                    cout << "HIJOS: " << closestEntryInNode->node->childs.size() << endl;
                }
            }
            else
                cout << "punto: " << closestEntryInNode->point->size() << endl;
            //cout << (*closestEntryInNode->point)[0] << ", " << (*closestEntryInNode->point)[1] << endl;
            closestEntry = closestEntryInNode;
            closestNode = node;
        }
        cout << "i: " << i++ << endl;
    }
    cout << "SALIO" << endl;
    return make_pair(closestEntry, closestNode);
}

void Node::addEntry(Entry* e) {
    if(e->value == 0){   
        this->points.push_back(*e->point);
    } else {
        this->childs.push_back(e->node);
    }
}

void Node::deleteEntry(const Entry* e) {
    int index, i = 0;
    if (e->value == 0) {
        for (auto point : this->points) {
            if (areEqualPoints(point, *e->point)) {  // comparar punto con punto
                index = i;
                break;
            }
            i++;
        }
        auto deleteIndex = this->points.begin() + index;
        this->points.erase(deleteIndex);
    } else {
        for (auto node : this->childs) {
            if (areEqualPoints(node->circle.center, e->node->circle.center)) {  // comparar centroide con punto
                index = i;
                break;
            }
            i++;
        }
        // borrar
        auto deleteIndex = this->childs.begin() + index;
        this->childs.erase(deleteIndex);
    }
}

Node* Node::findSiblingToMergeTo(Node* nodeToFix) {
    if (this->childs.size() < 2) return nullptr;

    double minDist = DBL_MAX;
    int idx;
    for (int i = 0; i < this->childs.size(); i++) {
        if (this->childs[i]->circle.center[0] == nodeToFix->circle.center[0] &&
            this->childs[i]->circle.center[1] == nodeToFix->circle.center[1]) {
            continue;
        }

        double dist = getDistance(this->childs[i]->circle.center, nodeToFix->circle.center);
        if (dist < minDist) {
            minDist = dist;
            idx = i;
        }
    }
    return this->childs[idx];
}

static void borrowFromSiblings(Node* node, vector<Node*> siblings) {
    auto e = findClosestEntryInNodesList(siblings, node);
    auto closestEntry = e.first;
    auto closestSibling = e.second;
    
    closestSibling->deleteEntry(closestEntry);
    updateBoundingEnvelope(closestSibling);
    node->addEntry(closestEntry);
    updateBoundingEnvelope(node);
}

static Node* merge(Node* firstNode, Node* secondNode) {
    //cout << "MergeChikito\n";
    if (firstNode->isLeaf == secondNode->isLeaf) {
        if (firstNode->isLeaf) {
            vector<Point> c;
            for (int i=0; i<firstNode->points.size(); i++) {
                c.push_back(firstNode->points[i]);
            }
            for (int i=0; i<secondNode->points.size(); i++) {
                c.push_back(secondNode->points[i]);
            }
            Node* newnode = new Node(true, c);
//////////////
            updateBoundingEnvelope(newnode);
//////////////
            return newnode;
        } else {
            vector<Node*> c;
            for (int i=0; i<firstNode->childs.size(); i++) {
                c.push_back(firstNode->childs[i]);
            }
            for (int i=0; i<secondNode->childs.size(); i++) {
                c.push_back(secondNode->childs[i]);
            }
            Node* newnode = new Node(false, c);
///////////////
            updateBoundingEnvelope(newnode);
///////////////
            return newnode;
        }
    }
    return nullptr;
}

void Node::mergeChildren(Node* firstChild, Node* secondChild) {
    cout << "Mergechildren\n";
    if (secondChild != nullptr) {
        Node* newchild = merge(firstChild, secondChild);
        for (int i=0; i<this->childs.size(); i++) {
            if (childs[i]->circle.center[0] == firstChild->circle.center[0] &&
                childs[i]->circle.center[1] == firstChild->circle.center[1]) {
                auto pos = this->childs.begin() + i;
                this->childs.erase(pos);
                break;
            }
        }
        for (int i=0; i<this->childs.size(); i++) {
            if (childs[i]->circle.center[0] == secondChild->circle.center[0] &&
                childs[i]->circle.center[1] == secondChild->circle.center[1]) {
                auto pos = this->childs.begin() + i;
                this->childs.erase(pos);
                break;
            }
        }

        this->childs.push_back(newchild);
    }
}

pair<bool, bool> recursiveRemove(Node* node, Point point) {
    bool deleted = false;
    Node* nodeToFix = nullptr;
    if (node->isLeaf) {
        bool areEqual = true;
        int index = -1;
        for (int i=0; i<node->points.size(); i++) {
            areEqual = areEqualPoints(node->points[i], point);
            if (areEqual) {             // Found
                index = i;
                break;
            }
        }
        if (areEqual && index != -1) {
            // Delete node
            auto delete_index = node->points.begin() + index;
            if (delete_index != node->points.end()) {
                cout << "DELETE: " << node->points[index][0] << " - " << node->points[index][1] << endl;
                node->points.erase(delete_index);
                ///
                updateBoundingEnvelope(node);
                ///
                return make_pair(true, node->points.size() < m);
            } else {
                cout << "Index out of range in \"Remove\" function\n";
            }
        } else {
            return make_pair(false, false);
        }
    } else {
        for (auto childNode : node->childs) {
            if (intersectsPoint(childNode, point)) {
                auto p = recursiveRemove(childNode, point);
                bool deleted = p.first;
                bool violatesInvariant = p.second;
                if (violatesInvariant) {
                    nodeToFix = childNode;
                }
                if (deleted)
                    break;
            }
        }
    }
    if (nodeToFix == nullptr) {
        if (deleted) {
            updateBoundingEnvelope(node);
        }
        return make_pair(deleted, false);
    } else {
        auto siblings = node->siblingsToBorrowFrom(nodeToFix);
        if (!siblings.empty()) {
            //cout << "BORROW FROM SIBLINGS" << endl;
            borrowFromSiblings(nodeToFix, siblings);
            //cout << "SALIO DEL BORROW FROM SIBLINGS" << endl;
        } else {
            cout << "MERGE CHILDREN" << endl;
            node->mergeChildren(nodeToFix, node->findSiblingToMergeTo(nodeToFix));
            // updateBoundingEnvelope(node);
        }

        updateBoundingEnvelope(node);

        return make_pair(true, node->childs.size() < m);
    }
}

void SSTree::remove(Point &point) {
    auto p = recursiveRemove(root, point);

    if (root->childs.size() == 1) {
        root = root->childs[0];
        //updateBoundingEnvelope(root);
    }
    
    /*cout << "AAAAAAAAAAAAAAAAAAAAAAAA" << endl;
    cout << "n of childs: " << this->root->childs.size() << endl;
    for (auto c : this->root->childs) {
        if (c->isLeaf) {
            cout << "child with " << c->points.size() << " points, radio = " << c->circle.radius << ", center = " << c->circle.center.size() << endl;
        } else {
            cout << "child with " << c->childs.size() << " childs" << endl;
            for (auto p : c->childs) {
                cout << "child with " << p->points.size() << " points, radio = " << p->circle.radius << c->circle.radius << ", center = " << c->circle.center.size() << endl;
            }
        }
    }*/
}

int colorIdx = 0;

void showNode(Node* node, cv::InputOutputArray &img) {
    if (node == nullptr) return;
    colorIdx++;
    if (node->isLeaf) {
        for (auto &p : node->points) {
            cv::circle(img, {(int)p[0], (int)p[1]}, radius, colors[3], -1);
        }
    }
    else {
        for (const auto &c : node->childs) {
            cv::circle(img, {(int)c->circle.center[0], (int)c->circle.center[1]}, (int)c->circle.radius, colors[colorIdx%6], 2);
            showNode(c, img);
        }
    }
}

void SSTree::show(cv::InputOutputArray &img) {
    colorIdx = 0;
    showNode(root, img);
}
