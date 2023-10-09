#include <iostream>
#include <string>
#include <vector>
#include "wlp4scan.h"
using namespace std;

int main() {
  std::string line;
  try {
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      for (auto &token : tokenLine) {
        std::cout << token << endl;
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  return 0;
}
