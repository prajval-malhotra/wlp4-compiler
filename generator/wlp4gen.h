#ifndef WLP4GEN_H
#define WLP4GEN_H

#include "tree.h"
#include <map>
#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

enum Type {
    INT=0, INT_STAR,
};

class Compiler {
	public:
		void compile(Node*);
		void printVariableTable();
		void generatePrologue(string);
		void generateEpilogue(string);
	private:
		int labelCount = 0;
		// fnName,        [arglist]
		map<string, vector<Type>> procedures;
		// fnName,      varName, varType
		map<string, map<string, Type>> variables;
		map<string, map<string, int>> varOffset;
		map<string, vector<string>> varOrder;
	
		void compileMain(Node*);
		void compileProcedure(Node*);
		void compileDcl(Node*, string, bool);
		void compileDcls(Node*, string);
		void compileStatements(Node*, string);
		void compileTest(Node*, string);
		void compileParams(Node*, string);
		Type compileExpr(Node*, string);
		Type compileTerm(Node*, string);
		Type compileFactor(Node*, string);
		Type compileLValue(Node*, string);
		Type compileFunctionWithArgs(Node*, string);
		Type compileArglist(Node*, string, int, string);
		
		void makeOffsetTable(string);
		void code(string, string);
		void push(int);
		void pop(int);
		void constantGenerator(string);
		string getUniqueLabel(string);

		Type getType(Node*);
		string getIDValue(Node*);

		bool variableExists(string, string);
		bool functionExists(string);
};

#endif
