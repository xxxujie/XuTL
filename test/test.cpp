#include <iostream>
#include <string>

#include "memory.h"

int main(int argc, char* argv[]) {
    // test codes;
    std::cout << __cplusplus << std::endl;
    std::string s{"test"};
    auto first = s.begin();
    auto last = s.end();
    int cnt{0};
    while (first++ != last) {
        std::cout << cnt++ << " ";
    }
    std::cout << std::endl;
    return 0;
}