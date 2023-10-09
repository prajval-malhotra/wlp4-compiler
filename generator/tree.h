#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
using namespace std;

class Node {
	public:
		string rule;
		vector<unique_ptr<Node>> children;
};

class Tree {
  public:
  	unique_ptr<Node> root;
    unique_ptr<Node> makeTree();
  private:
    bool isTerminal(string);
    vector<string> terminals = {
        "BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LE",
        "LPAREN", "LT", "MINUS", "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN",
        "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"
    };
};

#endif
