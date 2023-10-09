#include "wlp4parse.h"
using namespace std;

Parser::Parser() {}

/**
* Prints the contents of a vector to standard output for debugging purposes. 
* 
* @param arr - The vector to print
*/
void print(vector<string> arr) {
    cout << "Printing vector: " << endl;
    for(int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

/**
* Prints a vector of strings to standard output, useful for debugging
* 
* @param arr - The vector to print
*/
void printVecOfVecs(vector<vector <string>> arr) {
    for(int i = 0; i < arr.size(); i++) {
            cout << arr[i][0] << ": " << arr[i][1] << endl;
    }
}

/**
* Prints a map to std :: cout. 
* 
* @param myMap - the map to be printed
*/
void printMap(map<string, string> myMap) {
    map<string, string>::iterator it;

    for (it = myMap.begin(); it != myMap.end(); it++) {
        std::cout << it->first    // string (key)
        << ':'
        << it->second   // string's value 
        << std::endl;
    }
}

/**
* Reads input from file. 
* 
* @param f - the file to read from 
*/
void Parser::getInput(fstream &f) {
    int count = 0;
    int limit = 0;
    f >> limit; 
    while(count < limit) {
        string alph;
        f >> alph;
        terminals.push_back(alph);
        ++count;
    }
    
    count = 0;
    limit = 0;
    f >> limit; 
    while(count < limit) {
        string alph;
        f >> alph;
        nonTerminals.push_back(alph);
        ++count;
    }
    
    f >> start;
    
    count = 0;
    limit = 0;
    f >> limit;
    string ignore;
    getline(f, ignore);
    while(count < limit) {
        string symbol;
        string line;
        getline(f, line);
        stringstream stream(line);
        vector<string> temp;
        while(stream >> symbol) {
            temp.push_back(symbol);
        }
        productions.push_back(temp);
        ++count;
    }

    f >> numStates;
    f >> numTransitions;
    count = 0;
    while(count < numTransitions) {
        int state1, state2;
        string symbol, what;
        f >> state1 >> symbol >> what >> state2;
        if (what == "reduce") reductions[state1][symbol] = state2;
        else transitions[state1][symbol] = state2;
        count++;
    }
}

/**
* LR1 Parser
* 
* @param f - the stream to read
*/
void Parser::LR1(fstream &f) {
    getInput(f);
    stateStack.push(0);
    int bookmark = -1;
    string input;
    string input2;
    bool end = false;

    // 2: for each symbol a in ` x a from left to right do
    while(true) {
        if (bookmark == -1) {
            input = "BOF";
            input2 = "BOF";
        }
        else if (cin >> input) {
            cin >> input2;
        }
        else if (!end) {
            input = "EOF";
            input2 = "EOF";
            end = true;
        }
        else break;
            // 1: stateStack.push q0
        bookmark++;
        // 3: while Reduce[stateStack.top, a] is some production B → γ do
        while(reductions[stateStack.top()].find(input) != reductions[stateStack.top()].end()) {
            vector<string> rule = productions[reductions[stateStack.top()][input]];

            for(int i = 0; i < rule.size() - 1; i++) {
                // 4: symStack.pop symbols in γ
                symbolStack.pop();
                // 5: stateStack.pop |γ| states
                stateStack.pop();
            }
            // 6: symStack.push B
            symbolStack.push(rule[0]);

            for(int i = 0; i < rule.size(); i++) cout << rule[i] << " ";
            cout << endl;
            
            // 7: stateStack.push δ[stateStack.top, B]
            if(transitions[stateStack.top()].find(rule[0]) == transitions[stateStack.top()].end()) {
                cerr << "ERROR at " << bookmark << endl;
                return;
            }

            // 11: stateStack.push δ[stateStack.top, a]
            stateStack.push(transitions[stateStack.top()][rule[0]]);

        }  // 8: end while
        // 9: symStack.push a
        symbolStack.push(input);
        cout << input << " " << input2 << endl;

        // 10: reject if δ[stateStack.top, a] is undefined
        if(transitions[stateStack.top()].find(input) == transitions[stateStack.top()].end()) {
            cerr << "ERROR at " << bookmark << endl;
            return;
        }

        stateStack.push(transitions[stateStack.top()][input]);
    }  // 12: end for

    // 13: accept
    cout << start << " ";
    vector<string> reverse;
    while(!symbolStack.empty()) {
        reverse.push_back(symbolStack.top());
        symbolStack.pop();
    }
    for(int i = reverse.size() - 1; i >= 0; i--) {
        cout << reverse[i] << " ";
    }
    cout << endl;

}

    