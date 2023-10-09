#include "asm.h"
#include <vector>
using namespace std;

void printBinary(int64_t n) {
    unsigned char c = n >> 24;
    cout << c;
    c = n >> 16;
    cout << c;
    c = n >> 8;
    cout << c;
    c = n;
    cout << c;
}

unsigned char getBinary(int64_t n) {
    unsigned char c = n;
    return c;
}
