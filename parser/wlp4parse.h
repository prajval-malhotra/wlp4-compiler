#include <iostream>
#include <utility>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <fstream>
using namespace std;

class Parser {
        string start;
        int numStates;
        int numTransitions;

        vector<string> terminals;
        vector<string> nonTerminals;
        vector<string> alphabets;
        
        stack<int> stateStack;
        stack<string> symbolStack;

        vector<vector<string>> productions;
        map<int, map<string, int>> transitions;
        map<int, map<string, int>> reductions;
    public:
        Parser();
        void getInput(fstream &f);
        void checkVars();
        void LR1(fstream &f);
};
