#include "wlp4gen.h"
#include "tree.h"

/**
* Prints the procedures and variables table to standard error.
*/
void Compiler::printVariableTable() {
    for(auto out = procedures.begin(); out != procedures.end(); out++) {
        string function = out->first;
        // if(function != "wain") continue;
        std::cerr << function
                << ": ";
        for (auto i: out->second) {
            if(i == INT) std::cerr << "int" << ' ';
            else if(i == INT_STAR) std::cerr << "int*" << ' ';
        }
        cerr << endl;
        for (auto it = variables.begin(); it != variables.end(); ++it){
            if(it->first == function) {
                auto internal_map = it->second;
                // if(function != "wain") continue;
                for (auto it2 = internal_map.begin(); it2 != internal_map.end(); ++it2){
                        cerr << it2->first << " ";
                        if(it2->second == INT) std::cerr << "int";
                        else if(it2->second == INT_STAR) std::cerr << "int*";
                        cerr << endl;
                }
            }
        }
    }  
}

/**
* Checks if a variable exists in the compiler.
* 
* @param function - The name of the function
* @param var - The name of the variable
* 
* @return True if the variable exists false if it doesn't
*/
bool Compiler::variableExists(string function, string var) {
    if(variables[function].find(var) == variables[function].end()) return false;
    return true;
}

/**
* Checks if a function exists. 
* 
* @param function - The name of the function.
* 
* @return true if the function exists false otherwise. 
*/
bool Compiler::functionExists(string function) {
    if(procedures.find(function) == procedures.end()) return false;
    return true;
}

/**
* Returns the type of a node. This is used to determine how to handle type information that is stored in the AST.
* 
* @param node - The node to check. It must be a type node.
* 
* @return The type of the node.
*/
Type Compiler::getType(Node *node) {
    if (node->children.size() == 2) return INT_STAR;
    return INT;
}

/**
* Returns the ID value of the node. 
* 
* @param node - The node to get the ID value of.
* 
* @return The ID value of the node or null if the node doesn't have an ID
*/
string Compiler::getIDValue(Node *node) { return node->children[0]->rule; }

/**
* Creates and writes the offset table to cerr. 
* This is used to determine which variables are part of the function
*
* @param function - name of the function
*/
void Compiler::makeOffsetTable(string function) {
    map<string, int> newOffsets;
    cerr << "Offset Table:" << endl;
    for(auto out = procedures.begin(); out != procedures.end(); out++) {
        string function = out->first;
        std::cerr << function << ":" << endl;
        for (auto it = varOrder.begin(); it != varOrder.end(); ++it){
            if(it->first == function) {
                // auto internal_map = it->second;
                int numVars = it->second.size();
                int count = 0;
                // cerr << "num: " << numVars << endl;
                for(int i = 0; i < numVars; i++) {
                    string var = varOrder[function][i];
                    varOffset[function][var] = 4*count;
                    --count;
                    cerr << var << " ";
                    std::cerr << varOffset[function][var] << endl;
                }
            }
        }
    }
}

/**
* Generates asm prologue for function. 
* 
* @param function - Name of the function that is being compiled
*/
void Compiler::generatePrologue(string function) {
    cout << "; begin prologue:\n" << ".import print\n" 
    << ".import init\n"
    << ".import new\n"
    << ".import delete\n"
    << "lis $4 \t\t\t\t; $4 <- 4\n" 
    ".word 4\n" << "sub $29 , $30 , $4 \t; init frame pointer\n";
    cout << "lis $10\n.word print\n";
    cout << "lis $11\n.word 1\n";
}

/**
* Generate the asm epilogue for a function. This is used to add variables to the output file.
* 
* @param function - The name of the function that is being compiled
*/
void Compiler::generateEpilogue(string function) {
    cout << "\n; begin epilogue:" << endl;
    int numVars = varOffset[function].size();
    for(int i = 0; i < numVars; ++i) cout << "add $30 , $30 , $4\n";
    cout << "jr $31\n";
}

/**
* Prints code for a function. 
* 
* @param function - The name of the function. E. g. " c "
* @param variable - The name of the variable
*/
void Compiler::code(string function, string variable) {
    int offset = varOffset[function][variable];
    cout << "lw $3, " << offset << "($29)\n";
}

/**
* Asm for pushing a value onto the stack. 
* 
* @param reg - The register to push onto ( 0 - 3
*/
void Compiler::push(int reg) {
    cout << "sw $" << reg << " , -4($30)\n";
    cout << "sub $30, $30, $4\n";
}

/**
* Pop a value from a register. 
* 
* @param reg - The register to pop from.
*/
void Compiler::pop(int reg) {
    cout << "add $30, $30, $4\n";
    cout << "lw $" << reg << " , -4($30)\n";
}

/**
* Generates code that will be used to generate a constant.
* 
* @param value - The value of the constant to generate. Should be a string
*/
void Compiler::constantGenerator(string value) {
    cout << "lis $5\n";
    cout << ".word " << value << endl;
    // push(5);
}

/**
* Returns a unique label that is used to identify a function. This is the same 
* as label + 1 but can be used as a label for an anonymous function.
*
* @param label - The label to be used. Must not be null.
* 
* @return A unique label that is used to identify a function
*/
string Compiler::getUniqueLabel(string label) {
    ++labelCount;
    return label + to_string(labelCount);
}

/**
* Compile the AST from the given node. This is the top - level function called by main.
* 
* @param node - The node to compile
*/
void Compiler::compile(Node *node) {
    int size = node->children.size();
    // Compiles the procedures procedure main and procedures.
    for(int i = size-1; i >=0; i--) {
        if(node->children[i]->rule == "procedures") compile(node->children[i].get());
        else if(node->children[i]->rule == "procedure") compileProcedure(node->children[i].get());
        else if(node->children[i]->rule == "main") compileMain(node->children[i].get());
    }
}

/**
* compiles the wain (wlp4 main) program and prints the prologue to standard output
* 
* @param node - * pointer to the node to be examined
*/
void Compiler::compileMain(Node *node) {
    int count = 0;
    generatePrologue("wain");
    cout << "wain" << ":" << endl;
	cout << "sub $29, $30, $4" << endl;
    for(auto &it: node->children) {
        if(it->rule == "dcl") {
            count++;
            if(count == 1) {
                // if program is called with twoints, put 0 in $2
                push(2);
                if(it->children[0]->children.size() == 1) cout << "add $2, $0, $0\n";
                push(31);
                push(29);
                cout << "lis $5\n";
                cout << ".word init\n";
                cout << "jalr $5\n";
                pop(29);
                pop(31);
                pop(2);
            }
            if(count > 2) cerr << "SomethingNotRight: passing more than two args for wain" << endl;
            compileDcl(it.get(), "wain", true);
            if(count == 2 && procedures["wain"][1] != INT)
                cerr << "SomethingNotRight: second arg for wain cannot be of type \"INT*\" " << endl;
            push(count);
        }
        if(it->rule == "dcls") {
            compileDcls(it.get(), "wain");
            makeOffsetTable("wain");
            cout << "; end prologue\n";
        }
        if(it->rule == "statements") compileStatements(it.get(), "wain");
        if(it->rule == "expr") {
            if(compileExpr(it.get(), "wain") != INT) {
                cerr << "SomethingNotRight: wain must return type \"INT\"" << endl;
            }
            // generateEpilogue("wain");
        }
    }
    int numVars = varOffset["wain"].size();
    // cerr << "numvars: " << numVars << endl;
    for(int i = 0; i < numVars; ++i) pop(5);
    cout << "jr $31\n";
}

/**
* Compile a procedure and its subroutines.
* 
* @param node - * pointer to the node to
*/
void Compiler::compileProcedure(Node *node) {
    string id = getIDValue(node->children[1].get());
    if(functionExists(id)) 
        cerr << "SomethingNotRight: redeclaration of function: \"" << id << "\"" << endl;
    cout << id << ":\n";
    cout << "sub $29, $30, $4\n";
    procedures[id];
    for(auto &it: node->children) {
        if(it->rule == "params") compileParams(it.get(), id);
        if(it->rule == "dcls") {
            compileDcls(it.get(), id);
            makeOffsetTable(id);  
        }
        if(it->rule == "statements") compileStatements(it.get(), id);
        if(it->rule == "expr") {
        }
    }
    int numVars = varOffset[id].size();
    // cerr << "numvars: " << numVars << endl;
    for(int i = 0; i < numVars; ++i) pop(5);
    cout << "jr $31\n";
}

/**
* Compile parameters for a function. This is used to generate code that compiles the parameter list and DCL code for the function.
* 
* @param node - * The node to look for parameters in.
* @param id - The name of the function being compiled ( for debugging
*/
void Compiler::compileParams(Node* node, string id) {
    for(auto &it: node->children) {
        if(it->rule == "params") compileParams(it.get(), id);
        if(it->rule == "paramlist") compileParams(it.get(), id);
        if(it->rule == "dcl") compileDcl(it.get(), id, true);
    }
}

/**
* Compiles Declarations.
* 
* @param node - * pointer to node
* @param function - name of function that is being compiled
* @param isParam - true if function is a parameter false if not
*/
void Compiler::compileDcl(Node* node, string function, bool isParam) {
    for (auto &it: node->children) {
        Type type = getType(node->children[0].get());
        string id = getIDValue(node->children[1].get());
        if(variableExists(function, id)) cerr << "SomethingNotRight: redeclaration of variable \""
            << id << "\" in function \"" << function << "\"" << endl;
        map<string, Type> insert;

        varOrder[function].push_back(id);
        insert[id] = type;
        if(isParam) procedures[function].push_back(type);
        variables[function]
            .insert(pair<string, Type>(id, type));
        break;
    }
}

/**
* Compiles dcls into function. This is called recursively to compile subroutines
* 
* @param node - * pointer to top node of tree
* @param function - name of function to compile dcls into e. g
*/
void Compiler::compileDcls(Node* node, string function) {
    if(node->children.size() == 0) return;
    if (node->children[0]->rule == "dcls") {
        compileDcls(node->children[0].get(), function);
        auto cur = node->children[1].get();
        Type type = getType(cur->children[0].get());

        if (node->rule == "dcls" && node->children.size() == 5) {
            if(node->children[3]->rule == "NUM") {
                if(type != INT) cerr << "SomethingNotRight: cannot assign \"INT\" value to \"INT*\"" << endl;
                // cerr << "num: " << node->children[3]->children[0]->rule << endl;
                string constant = node->children[3]->children[0]->rule;
                constantGenerator(constant);
                push(5);
            }
            else if(node->children[3]->rule == "NULL") {
                // string constant = node->children[3]->children[0]->rule;
                // constantGenerator(constant);
                // cout << "add $3, $0, $11\n";
                push(11);
                if(type != INT_STAR) cerr << "SomethingNotRight: cannot assign \"INT*\" value to \"INT\"" << endl;
            }
        }
        string variable = getIDValue(cur->children[1].get());

        if(variableExists(function, variable)) cerr << "SomethingNotRight: redeclaration of variable \""
            << variable << "\" in function \"" << function << "\"" << endl;

        varOrder[function].push_back(variable);
        map<string, Type> insert;
        insert[variable] = type;
        variables[function]
            .insert(pair<string, Type>(variable, type));
    }
}

/**
* Compiles the statements in a node. This is used to test the compilation of IF and ELSE statements
* 
* @param node - * The node to compile.
* @param function - The function being compiled ( " if " " elif " etc.
*/
void Compiler::compileStatements(Node* node, string function) {
    if(node->children.size() == 0) return;
    if(node->children[0]->rule == "statements") {
        compileStatements(node->children[0].get(), function);
        compileStatements(node->children[1].get(), function);    
    }
    if(node->children[0]->rule == "IF") {
        string elseLabel = getUniqueLabel("else");
        string end = getUniqueLabel("endif");
        compileTest(node->children[2].get(), function);
        cout << "beq $3, $0, " << elseLabel << endl;
        compileStatements(node->children[5].get(), function);
        cout << "beq $0, $0, " << end << endl;
        cout << elseLabel << ":\n";
        compileStatements(node->children[9].get(), function);
        cout << end << ":\n";
    }
    if(node->children[0]->rule == "WHILE") {
        string loop = getUniqueLabel("loop");
        string endWhile = getUniqueLabel("endWhile");
        cout << loop << ":\n";
        compileTest(node->children[2].get(), function);
        cout << "beq $3, $0, " << endWhile << endl;
        compileStatements(node->children[5].get(), function);
        cout << "beq $0, $0, " << loop << endl;
        cout << endWhile << ":\n";
    }
    if(node->children[0]->rule == "PRINTLN") {
        push(1);
        if(compileExpr(node->children[2].get(), function) != INT)
            cerr << "SomethingNotRight: \"println\" cannot be used with type \"INT*\"" << endl;
        cout << "add $1, $3, $0\n";
        push(31);
        cout << "lis $5\n.word print\njalr $5\n";
        pop(31);
        pop(1);
    }
    if(node->children[0]->rule == "DELETE") {
        Type exprRetval = compileExpr(node->children[3].get(), function);
        string label = getUniqueLabel("skipDelete");
        cout << "beq $3, $11, " << label << endl;
        cout << "add $1, $3, $0\n";
        push(31);
        cout << "lis $5\n";
        cout << ".word delete\n";
        cout << "jalr $5\n";
        pop(31);
        cout << label << ":" << endl;

        if(exprRetval != INT_STAR)
            cerr << "SomethingNotRight: \"delete\" cannot be used with type \"INT\"" << endl;
    }
    if(node->children[0]->rule == "lvalue") {
        Type right = compileExpr(node->children[2].get(), function);
        cout << "; here\n";
        push(3);
        Type left = compileLValue(node->children[0].get(), function);
        pop(5);
        string variable = node->children[0]->children[0]->children[0]->rule;
        
        int offset = varOffset[function][variable];
        if(node->children[0]->children.size() == 1) cout << "sw $5, " << offset << " ($29)\n" << endl;
        else if(node->children[0]->children.size() == 2) cout << "sw $5, 0($3)" << endl;
        if(left != right) {
            cerr << "SomethingNotRight: lvalue does not match " << endl;
        }
    }
    for (auto &it: node->children) {
        if(it->rule == "test") compileTest(it.get(), function);
        // if(it->rule == "statements") compileStatements(it.get(), function);
    }
}

/**
* Compiles an lvalue and returns the type of the result.
* 
* @param node - * pointer to the node that is going to be compiled
* @param function - name of the function that is being compiled
* 
* @return the type of the lvalue or INT if there is
*/
Type Compiler::compileLValue(Node* node, string function) {
    if(node->children.size() == 1) {
        string variable = getIDValue(node->children[0].get());
        return variables[function][variable];
    }
    if(node->children.size() == 2) {
        cout << ";in lvalue\n";
        Type retval = compileFactor(node->children[1].get(), function);
        if(retval == INT_STAR) return INT;
        cerr << "SomethingNotRight: cannot dereference an integer" << endl;
    }
    if(node->children.size() == 3) return compileLValue(node->children[1].get(), function);
}

/**
* Compiles testing expressions and outputs the result to standard output. 
* 
* @param node - * The node to compile. This is the root of the tree being compiled.
* @param function - The function being compiled
*/
void Compiler::compileTest(Node* node, string function) {
    Type left = compileExpr(node->children[0].get(), function);
    push(3);
    string middle = node->children[1]->rule;
    Type right = compileExpr(node->children[2].get(), function);
    pop(5);

    string what;
    if(left == INT_STAR) what = "sltu";
    else if(left == INT) what = "slt";

    if(middle == "LT") cout << what << " $3, $5, $3\n";
    if(middle == "GT") cout << what << " $3, $3, $5\n";
    if(middle == "GE") {
        cout << what << " $3, $5, $3\n";
        cout << "sub $3, $11, $3\n";
    }
    if(middle == "LE") {
        cout << what << " $3, $3, $5\n";
        cout << "sub $3, $11, $3\n";
    }
    if(middle == "NE") {
        cout << "slt $6, $3, $5\t\t\t\t\t; $6 = $3 < $5\n";
        cout << "slt $7, $5, $3\t\t\t\t\t; $7 = $5 < $3\n";
        cout << "add $3, $6, $7\n";
    }
    if(middle == "EQ") {
        cout << "slt $6, $3, $5\t\t\t\t\t; $6 = $3 < $5\n";
        cout << "slt $7, $5, $3\t\t\t\t\t; $7 = $5 < $3\n";
        cout << "add $3, $6, $7\n";
        cout << "sub $3, $11, $3\n";
    }
    if(left != right) {
        cerr << "SomethingNotRight: expression comparison failed: cannnot compare type \"" 
            << left << "\" with \""
            << right << "\"" << endl;
    }
}

/**
* Compiles a factor and returns the type of the result.
* 
* @param node - * pointer to the node that has to be compiled
* @param function - name of the function that is being compiled
* 
* @return type of the result of the factor as it is
*/
Type Compiler::compileFactor(Node *node, string function) {
    if(node->children[0]->rule == "NULL") {
        // cout << "lis $5\n";
        // cout << ".word 0x01"
        cout << "add $3, $0, $11\n";
        // push(11);
        return INT_STAR;
    }
    
    if(node->children[0]->rule == "NUM") {
        constantGenerator(node->children[0]->children[0]->rule);
        cout << "add $3, $5, $0\n";
        return INT;
    }
    if(node->children[0]->rule == "STAR") {
        Type left = compileFactor(node->children[1].get(), function);
        cout << "lw $3, 0($3)\n";
        if(left == INT_STAR) return INT;
        else cerr << "SomethingNotRight: cannot use * with type \"INT\"" << endl;
    }
    if(node->children[0]->rule == "AMP") {
        if(node->children[1]->children.size() == 1) {
            cout << "lis $3\n";
            string variable = node->children[1]->children[0]->children[0]->rule;
            int offset = varOffset[function][variable];
            cout << ".word " << offset << endl;
            cout << "add $3, $3, $29\n";
        }
        if(compileLValue(node->children[1].get(), function) == INT)
            return INT_STAR;
        else cerr << "SomethingNotRight: cannot use & with type \"INT*\"" << endl;
    }
    if(node->children[0]->rule == "LPAREN") return compileExpr(node->children[1].get(), function);
    
    if(node->children[0]->rule == "NEW") {
        cout << ";   new\n";
        if(compileExpr(node->children[3].get(), function) == INT) {
            cout << " add $1, $3, $0\n";
            push(31);
            cout << "lis $5\n";
            cout << ".word new\n";
            cout << "jalr $5\n";
            pop(31);
            cout << "bne $3, $0, 1\n";
            cout << "add $3, $11, $0\n";
            return INT_STAR;
        }
        else cerr << "SomethingNotRight: \"new\" can only be used with type \"INT\"" << endl;
    }
    if(node->children.size() == 1) {
        string variable = getIDValue(node->children[0].get());
        if(!variableExists(function, variable)) {
            cerr << "SomethingNotRight: variable \""
            << variable << "\" not declared in function \"" << function << "\"" << endl;
        }
        // cerr << "func: " << variable << endl;
        code(function, variable);
        return variables[function][variable];
    }
    if(node->children.size() == 3) {
        string callingFunction = getIDValue(node->children[0].get());
        push(31);
        push(29);
        cout << "lis $5\n";
        cout << ".word " << callingFunction << endl;
        cout << "jalr $5\n";
        pop(29);
        pop(31);
        if(functionExists(callingFunction)) {
            if(procedures[callingFunction].size() == 0) return INT;
            else cerr << "SomethingNotRight: Wrong number of arguments passed to \"" << callingFunction
                << "\"" << endl;
        }
        else cerr << "SomethingNotRight: function \"" << callingFunction
            << "\" not declared" << endl;
    }
    if(node->children.size() == 4) {
        string callingFunction = getIDValue(node->children[0].get());
        push(29);
        push(31);
        Type retval = compileFunctionWithArgs(node, function);
        cout << "lis $5\n";
        cout << ".word " << callingFunction << endl;
        cout << "jalr $5\n";
        pop(31);
        pop(29);

        return retval;
    }
}

/**
* Compiles the arguments list
* 
* @param node - * pointer to the node that is being compiled
* @param function - name of the function that is being compiled
* @param numArgs - number of arguments in the list
* @param callingFunction - name of the function that is calling the function
* 
* @return type of the first argument or null if there is
*/
Type Compiler::compileArglist(Node *node, string function, int numArgs, string callingFunction) {
    if(!functionExists(callingFunction)) {
        cerr << "SomethingNotRight: function \"" << callingFunction << "\" not declared" << endl;
    }
    else if((numArgs-1) >= procedures[callingFunction].size()) {
        cerr << "SomethingNotRight: too many arguments passed to \"" << callingFunction
                << "\"" << endl;
    }
    else {
        if(node->children.size() == 1) {
            if((numArgs == 1)) {
                if((numArgs) != procedures[callingFunction].size())
                    cerr << "SomethingNotRight: too few arguments passed to \"" << callingFunction
                    << "\"" << endl;
            }

            Type retval = compileExpr(node->children[0].get(), function);
            push(3);
            if(procedures[callingFunction][numArgs-1] != retval)
            cerr << "SomethingNotRight: wrong type passed as arg to function: \"" << callingFunction
                << "\"" << endl;
            return INT; 
        }
        if(node->children.size() == 3) {
            if((node->children[2]->children.size() == 1)) {
                if((numArgs+1) != procedures[callingFunction].size())
                    cerr << "SomethingNotRight: too few arguments passed to \"" << callingFunction
                    << "\"" << endl;
            }
            Type retval = compileExpr(node->children[0].get(), function);
            if(procedures[callingFunction][numArgs-1] != retval)
            cerr << "SomethingNotRight: wrong type passed as arg to function: \"" << callingFunction
                << "\"" << endl;
            else {
                return compileArglist(node->children[2].get(), function, numArgs+1, callingFunction);
            }
        }
    }
}

/**
* Compiles a function with arguments. 
* 
* @param node - * The node that contains the function to compile
* @param function - The name of the function to compile
* 
* @return The type of the function with arguments
*/
Type Compiler::compileFunctionWithArgs(Node *node, string function) {
    string callingFunction = getIDValue(node->children[0].get());
    if(!functionExists(callingFunction)) cerr << "SomethingNotRight: function \"" << callingFunction
        << "\" not declared" << endl;
    return compileArglist(node->children[2].get(), function, 1, callingFunction);
}

/**
* Compiles a term and its subterms.
* 
* @param node - * The node that is to be compiled.
* @param function - The name of the function that is being compiled.
* 
* @return The type of the expression that is represented by the term
*/
Type Compiler::compileTerm(Node *node, string function) {
    if(node->children.size() == 1) 
        return compileFactor(node->children[0].get(), function);
    if(node->children.size() == 3) {
        Type left = compileTerm(node->children[0].get(), function);
        push(3);
        string middle = node->children[1]->rule;
        Type right = compileFactor(node->children[2].get(), function);
        pop(5);
        if(middle == "STAR") {
            cout << "mult $5, $3\n";
            cout << "mflo $3\n";
        }
        if(middle == "SLASH") {
            cout << "div $5, $3\n";
            cout << "mflo $3\n";
        }
        if(middle == "PCT") {
            cout << "div $5, $3\n";
            cout << "mfhi $3\n";
        }
        if(left == right && left == INT) return INT;
        cerr << "SomethingNotRight: cannot compare \"" << left
            << "\" to \"" << right << "\"" << endl;
    }
}

/**
* Compiles an expression and returns the type of the result. 
* 
* @param node - The node to compile.
* @param function - The function to compile.
* 
* @return The type of the result of the expression. 
*/
Type Compiler::compileExpr(Node *node, string function) {
    if(node->children.size() == 1) {
        Type retval = compileTerm(node->children[0].get(), function);
        return retval;
    }
    if(node->children.size() == 3) {
        string middle = node->children[1]->rule;
        Type left = compileExpr(node->children[0].get(), function);
        push(3);
        Type right = compileTerm(node->children[2].get(), function);
        pop(5);
        if(left == INT && right == INT) {
            if(middle == "PLUS") cout << "add $3, $5, $3\n";
            if(middle == "MINUS") cout << "sub $3, $5, $3\n";
            return INT;
        }
        if(left == INT_STAR && right == INT) {
            cout << "mult $3, $4\n";
            cout << "mflo $3\n";
            if(middle == "PLUS") cout << "add $3, $5, $3\n";
            if(middle == "MINUS") cout << "sub $3, $5, $3\n";
            return INT_STAR;
        }
        if(left == INT && right == INT_STAR) {
            cout << "mult $5, $4\n";
            cout << "mflo $5\n";
            if(middle == "PLUS") cout << "add $3, $5, $3\n";
            return INT_STAR;
        }
        if(left == INT_STAR && right == INT_STAR && middle == "MINUS") {
            cout << "sub $3, $5, $3\n";
            cout << " div $3, $4\n";
            cout << "mflo $3\n";
            return INT;
        }
        // if(left == INT && right == INT_STAR && middle == "PLUS") return INT_STAR;
        // if(left == INT && right == INT) return INT;
        cerr << "SomethingNotRight: expression comparison invalid" << endl;
    }
    
}