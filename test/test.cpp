#include <iostream>
#include <string>

#include "vector.h"

int main(int argc, char* argv[]) {
    // test codes
    xutl::vector<int> v;
    int i = 2;
    v.push_back(i);
    v.push_back(3);
    v.push_back(4);
    /* bug when done push_back(T&&)*/
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.pop_back();
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    return 0;
}