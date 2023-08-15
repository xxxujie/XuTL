#include <iostream>
#include <string>

#include "vector.h"

int main(int argc, char* argv[]) {
    // test codes
    xutl::vector<int> v;
    int i = 2;
    v.push_back(i);
    v.push_back(3);
    v.emplace_back(4);
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.pop_back();
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.assign(5, 1);
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.insert(v.begin() + 1, 2);
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.insert(v.begin() + 2, 2, 3);
    for (const auto& i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    v.reserve(100);
    std::cout << v.capacity() << std::endl;
    v.shrink_to_fit();
    std::cout << v.capacity() << std::endl;
    return 0;
}