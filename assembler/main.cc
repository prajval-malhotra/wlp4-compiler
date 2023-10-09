#include "asm.h"
#include "utility.h"
#include <cstring>
using namespace std;

//       0:  ID,
//       1:  LABEL,
//       2:  WORD,
//       3:  COMMA,
//       4:  LPAREN,
//       5:  RPAREN,
//       6:  INT,
//       7:  HEXINT,
//       8:  REG,
//       9:  WHITESPACE,
//       10: COMMENT

void printBinaryEight(int64_t n) {
    unsigned char c = n >> 8;
    cout << c;
}

void printArgsBinary(int64_t op, int64_t s, int64_t t, int64_t d, int64_t func) {
    int instr = (op<<26) | (s<<21) | (t<<16) | (d<<11) | func; // add $3, $2, $4
    unsigned char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr >> 0;
    cout << c;

}

void printBranchLabel(int64_t op, int64_t s, int64_t t, int64_t i) {
    // int instr =  (op<<26) | (s<<21) | (t<<16) | (i<<0); // add $3, $2, $4
    int instr =  (op<<26) | (s<<21) | (t<<16) | i; // add $3, $2, $4
    unsigned char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr << 0;
    cout << c;
}

void printBranchBinary(int64_t op, int64_t s, int64_t t, int64_t i) {
    int instr = (op<<26) | (s<<21) | (t<<16) | i;
    if(i < 0) {
        instr = (op<<26) | (s<<21) | (t<<16) | (i & 0xffff);
    }
     // add $3, $2, $4
    unsigned char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr >> 0;
    cout << c;
}

void printLabels(vector<int> address, vector<string> labels) {
    int n = labels.size();

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(i == j) continue;
            if(labels[i] == labels[j])
                throw ScanningFailure("ERROR: duplicate labels not allowed");
        }
    }

    for(int i = 0; i < n; i++) {
        cerr << labels[i] << " " << address[i] << endl;
    }
}

// returns -1 if label not in labels, else returns index
int getLabel(vector<string> labels, string label) {
    int n = labels.size();

    for(int i = 0; i < n; i++) {
        if(labels[i] == label) return i;
    }
    return -1;
}

int main() {
    std::string line;
    vector<vector<Token>> store;
    vector<int> address;
    vector<string> labels;
    int labelcount = 0;
    int numline = 0;
    try {
        while (getline(std::cin, line)) {
            if(line == "") continue;
            // cout << numline << "line:" << line << endl;
            std::vector<Token> tokenLine = scan(line);
            if(line[0] == ';') continue;
            store.push_back(tokenLine);
            int branchHex = 0;
            for(int i = 0; i < tokenLine.size(); i++) {
                // cout << "Token: " << tokenLine[i].getKind() << endl;
                int what = tokenLine[i].getKind();

                string lex = tokenLine[i].getLexeme();
                int lineSize = tokenLine.size();

                if(what == 8) {
                    int r_no = tokenLine[i].toNumber();
                    if(r_no < 0 || r_no > 31)
                        throw ScanningFailure("ERROR: register number must be between 0 and 31");
                }

                if(what == 1) {  // labels
                    // cout << "label: " << labelcount << " " << tokenLine.size() << endl;
                    int addr = numline * 4;
                    address.push_back(addr);
                    string l = tokenLine[i].getLexeme();
                    l.pop_back();
                    labels.push_back(l);
                    ++labelcount;
                    if(tokenLine.size() == 1) --numline;
                }

                else if(lex == ".word") {
                    if(tokenLine.size() != (i + 2)) 
                        throw ScanningFailure("ERROR: too many arguments for .word");
                    if(!(tokenLine.size() >= (i+1)))
                        throw ScanningFailure("ERROR: need an argument for .word");
                    int nextKind = tokenLine[i+1].getKind();
                    // if(nextKind != 6 && nextKind != 7) {
                    if(nextKind != 6 && nextKind != 7 && nextKind != 0) {
                        // cout << nextKind << endl;
                        throw ScanningFailure("ERROR: not a valid arg for .word");
                    }
                    int64_t num = tokenLine[i + 1].toNumber();
                    if((nextKind == 6) && ((num > INT_MAX) || (num < INT_MIN))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((nextKind == 7) && (num > 4294967295)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");
                    // printBinary(num);
                    i += 1;
                }

                else if(lex == "jr" || lex == "jalr") {
                    if((tokenLine.size() <= (i+1)))
                        throw ScanningFailure("ERROR: need a register argument for jr/jalr");
                    if((i+1) != tokenLine.size()-1) 
                        throw ScanningFailure("ERROR: expected end of line but found more stuff");

                    int nextKind = tokenLine[i+1].getKind();
                    if(nextKind != 8) {
                        throw ScanningFailure("ERROR: not a valid arg for jr/jalr");
                    }
                    
                    // cout << tokenLine.size() << endl;
                }
                else if(lex == "add" || lex == "sub" || lex == "slt" || lex == "sltu") {
                    int s = tokenLine.size();
                    if(s != i + 6) 
                        throw ScanningFailure("ERROR: need a register argument");
                    if((tokenLine[i+1].getKind() != 8) ||(tokenLine[i+3].getKind() != 8) ||(tokenLine[i+5].getKind() != 8))
                        throw ScanningFailure("ERROR: invalid argument");
                }
                else if((what == 1) && lex == "beq" || lex == "bne") {
                    // cout << "hsjdhdsjdhs" << endl;
                    int s = tokenLine.size();
                    if(s != i + 6) 
                        throw ScanningFailure("ERROR: need a register argument");
                    if((tokenLine[i+1].getKind() != 8) ||(tokenLine[i+3].getKind() != 8) ||
                        ((tokenLine[i+5].getKind() != 7) && (tokenLine[i+5].getKind() != 6) &&
                         (tokenLine[i+5].getKind() != 0)))
                        throw ScanningFailure("ERROR: invalid argument");
                    branchHex++;

                    
                    int64_t num = tokenLine[i + 1].toNumber();
                    if((tokenLine[i+1].getKind() == 6) && ((num > 32767) || (num < -32768))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((tokenLine[i+1].getKind() == 7) && (num > 65535)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");
                    
                    num = tokenLine[i + 3].toNumber();
                    if((tokenLine[i+3].getKind() == 6) && ((num > 32767) || (num < -32768))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((tokenLine[i+3].getKind() == 7) && (num > 65535)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");
                    
                    num = tokenLine[i + 5].toNumber();
                    if((tokenLine[i+5].getKind() == 6) && ((num > 32767) || (num < -32768))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((tokenLine[i+5].getKind() == 7) && (num > 65535)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");

                    // num = tokenLine[i + 5].toNumber();
                    if((tokenLine[i+5].getKind() == 6) && ((num > INT_MAX) || (num < INT_MIN))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((tokenLine[i+5].getKind() == 7) && (num > 65535)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");
                    // printBinary(num);
                }
                else if(lex == "lis" || lex == "mflo" || lex == "mfhi") {
                    if((tokenLine.size() <= (i+1)))
                        throw ScanningFailure("ERROR: need a register argument for lis");
                    if((i+1) != tokenLine.size()-1) 
                        throw ScanningFailure("ERROR: expected end of line but found more stuff");

                    int nextKind = tokenLine[i+1].getKind();
                    if(nextKind != 8) {
                        throw ScanningFailure("ERROR: not a valid arg for lis");
                    }
                }
                else if(lex == "div" || lex == "divu" || lex == "mult" || lex == "multu") {
                    int s = tokenLine.size();
                    if(s != i + 4) 
                        throw ScanningFailure("ERROR: need a register argument");
                    if((tokenLine[i+1].getKind() != 8) ||(tokenLine[i+3].getKind() != 8))
                        throw ScanningFailure("ERROR: invalid argument");
                    // if((tokenLine.size() > (i+4))) {
                    //     cout << "i: " << i << " + 4: " << i + 4 << ", size: " << tokenLine.size() << endl;
                    //     throw ScanningFailure("ERROR: expected end of line but found more stuff");
                    // }
                }
                // else if(lex == "mult") {}
                // else if(lex == "multu") {}
                // else if(lex == "div") {}
                // else if(lex == "divu") {}
                else if(lex == "lw" || lex == "sw") {
                    int s = tokenLine.size();
                    if(s != i + 7) 
                        throw ScanningFailure("ERROR: invalid number of arguments");
                    if((tokenLine[i+1].getKind() != 8) || (tokenLine[i+2].getKind() != 3) ||
                        ((tokenLine[i+3].getKind() != 6) && (tokenLine[i+3].getKind() != 7))
                         || (tokenLine[i+4].getKind() != 4) ||
                        (tokenLine[i+5].getKind() != 8) || (tokenLine[i+6].getKind() != 5))
                        throw ScanningFailure("ERROR: invalid argument");
                    

                    
                    int64_t num = tokenLine[i + 3].toNumber();
                    if((tokenLine[i+3].getKind() == 6) && ((num > 32767) || (num < -32768))) 
                        throw ScanningFailure("ERROR: integer value entered out of bounds");
                    if((tokenLine[i+3].getKind() == 7) && (num > 65535)) 
                        throw ScanningFailure("ERROR: hexadecimal value entered out of bounds");
                }

                else if(what == 10) {
                    // if(tokenLine.size() > 0) {
                    //     if(tokenLine[0].get )
                    // }
                    // cout << "fkdjbhsdjkf" << endl;
                }

            }
            ++numline;
        }

        // labelcount = 0;
        // numline = 0;
        numline = 0;
        int programLength = store.size();
        // cout << "hi: " << endl << endl;
        // // while (getline(std::cin, line)) {
        for(int k = 0; k < programLength; k++) {
            std::vector<Token> fakeTokenLine = store[k];
            for(int i = 0; i < fakeTokenLine.size(); i++) {
                int what = fakeTokenLine[i].getKind();

                string lex = fakeTokenLine[i].getLexeme();
                int lineSize = fakeTokenLine.size();

                if(what == 1) {}

                else if(lex == ".word" && what != 1) {

                    int64_t num = fakeTokenLine[i + 1].toNumber();

                    int nextKind = fakeTokenLine[i+1].getKind();
                    if(nextKind == 0) {
                        num = getLabel(labels, fakeTokenLine[i+1].getLexeme());
                        if(num == -1)
                            throw ScanningFailure("ERROR: not a valid label");
                        else {
                            int temp = address[num];
                            printBinary(temp);
                        }
                        
                    }
                    // cout << "temp: " << temp << endl;
                    else {
                        printBinary(num);
                    }
                    i += 1;
                }

                else if(lex == "jr") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), 0, 0, 8);
                    ++i;
                }
                else if(lex == "jalr") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), 0, 0, 9);
                    ++i;
                }
                else if(lex == "slt") {
                    printArgsBinary(0, fakeTokenLine[i+3].toNumber(), fakeTokenLine[i+5].toNumber(), 
                    fakeTokenLine[i+1].toNumber(), 42);
                    i += 5;
                }
                else if(lex == "sltu") {
                    printArgsBinary(0, fakeTokenLine[i+3].toNumber(), fakeTokenLine[i+5].toNumber(), 
                    fakeTokenLine[i+1].toNumber(), 43);
                    i += 5;
                }
                else if(lex == "add") {
                    printArgsBinary(0, fakeTokenLine[i+3].toNumber(), fakeTokenLine[i+5].toNumber(), 
                    fakeTokenLine[i+1].toNumber(), 32);
                    i += 5;
                }
                else if(lex == "sub") {
                    printArgsBinary(0, fakeTokenLine[i+3].toNumber(), fakeTokenLine[i+5].toNumber(), 
                    fakeTokenLine[i+1].toNumber(), 34);
                    i += 5;
                }
                else if(lex == "beq" && what != 1) {
                    if(fakeTokenLine[i+5].getKind() == 0) { // label
                        int index = getLabel(labels, fakeTokenLine[i+5].getLexeme());
                        int labelValue = address[index];
                        int offset = 0;
                        offset = (labelValue - (numline * 4)) / 4;
                        // cout << offset << endl;
                        // cout << "index: " << offset << endl;

                        printBranchBinary(4, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                           offset);
                        // printBranchBinary(4, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                        //    offset);
                    }

                    else {
                        printBranchBinary(4, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                          fakeTokenLine[i+5].toNumber());
                    }
                    i += 5;
                }
                else if(lex == "bne" && what != 1) {
                    // (labelValue-PC)/4

                    if(fakeTokenLine[i+5].getKind() == 0) {
                        int index = getLabel(labels, fakeTokenLine[i+5].getLexeme());
                        int labelValue = address[index];
                        int offset = 0;
                        offset = (labelValue - (numline * 4)) / 4;

                        printBranchBinary(5, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                           offset);
                        // printBranchBinary(4, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                        //    offset);
                    }

                    else {
                        printBranchBinary(5, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                          fakeTokenLine[i+5].toNumber());
                    }
                    i += 5;
                }
                else if(lex == "lis") {
                    printArgsBinary(0, 0, 0, fakeTokenLine[i+1].toNumber(), 20);
                    ++i;
                }
                else if(lex == "mflo") {
                    printArgsBinary(0, 0, 0, fakeTokenLine[i+1].toNumber(), 18);
                    ++i;
                }
                else if(lex == "mfhi") {
                    printArgsBinary(0, 0, 0, fakeTokenLine[i+1].toNumber(), 16);
                    ++i;
                }
                else if(lex == "mult") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                    0, 24);
                    i += 3;
                }
                else if(lex == "multu") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                    0, 25);
                    i += 3;
                }
                else if(lex == "div") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                    0, 26);
                    i += 3;
                }
                else if(lex == "divu") {
                    printArgsBinary(0, fakeTokenLine[i+1].toNumber(), fakeTokenLine[i+3].toNumber(), 
                    0, 27);
                    i += 3;
                }

                else if(lex == "lw") {
                    printBranchBinary(35, fakeTokenLine[i+5].toNumber(), fakeTokenLine[i+1].toNumber(), 
                        fakeTokenLine[i+3].toNumber());
                    i += 6;
                }
                else if(lex == "sw") {
                    printBranchBinary(43, fakeTokenLine[i+5].toNumber(), fakeTokenLine[i+1].toNumber(), 
                        fakeTokenLine[i+3].toNumber());
                    i += 6;
                }

                else if((what == 0 && what != 10) || what == 7){
                    // cout << "lex: " << endl << lex << endl;
                    throw ScanningFailure("ERROR: invalid string");
                }

            } 
            ++numline;
        }
        printLabels(address, labels);
    } 
    catch (ScanningFailure &f) {
        std::cerr << f.what() << std::endl;
        return 1;
    }

    return 0;
} 