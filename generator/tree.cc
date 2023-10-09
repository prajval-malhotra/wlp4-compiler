#include "tree.h"

/**
* Checks if toFind is a terminal. This is used to find out if a tree node is part of a tree
* 
* @param toFind - The string to look for
* 
* @return true if toFind is a terminal false if not or if it is not a terminal in the termin
*/
bool Tree::isTerminal(string toFind) {
    for(int i = 0; i < terminals.size(); i++) {
        if(toFind == terminals[i]) return true;
    }
    return false;
}

/**
* Create a tree from input. This is used to generate an automaton that can be fed to a tree
*/
unique_ptr<Node> Tree::makeTree() {
    int val, numChildren = 0;
    string input, lhs, rhs;
    getline(cin, input);
    stringstream line{input};
    line >> lhs;
    auto curnode = make_unique<Node>();
    curnode->rule = lhs;
    while(line >> rhs) {
        numChildren++;
    }
    if(isTerminal(lhs)) {
        auto leaf = make_unique<Node>();
        leaf->rule = rhs;
        curnode->children.push_back(move(leaf));
    }
    else {
        while(numChildren > 0) {
            curnode->children.push_back(makeTree());
            --numChildren;
        }
    }
    return move(curnode);
}