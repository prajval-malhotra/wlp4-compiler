#include "wlp4gen.h"
#include "tree.h"

int main() {
    Compiler compiler;
    auto tree = make_unique<Tree>();
    tree->root = tree->makeTree();
    // compiler.generatePrologue();
    compiler.compile(tree->root.get());
    // compiler.generateEpilogue();
    compiler.printVariableTable();
}
