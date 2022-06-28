//
// Created by Sebastian Knell on 28/06/22.
//

#include "SSTree.h"

Node::~Node() {
    if (!isLeaf) {
        for (auto &c : childs) delete c;
    }
}

void SSTree::insert(Point point) {

}

void SSTree::remove(Point point) {

}
