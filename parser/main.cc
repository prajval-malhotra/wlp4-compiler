#include "wlp4parse.h"
using namespace std;

int main() {
    fstream f ("grammar.txt");
    Parser parser;
    parser.LR1(f);
    return 0;
}