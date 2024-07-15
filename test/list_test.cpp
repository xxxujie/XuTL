#include <cstdio>

#include "list.h"

int main() {
    xutl::list<int> li;
    printf("size = %ld\n", li.size());
    li.push_back(0);
    li.push_back(1);
    li.push_back(2);
    li.push_back(3);
    li.push_back(4);
    printf("size = %ld\n", li.size());
    return 0;
}