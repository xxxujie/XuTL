#ifndef XUTL_ALLOCATOR_H
#define XUTL_ALLOCATOR_H

#include <stddef.h>

#include <iostream>

/**
 * 这是模板类 allocator
 * 用于分配和释放内存，构造和析构对象
 */

namespace xutl {

template <class T>
class allocator {
public:
    // 相关类型
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    pointer allocate() {
        return static_cast<pointer>(::operator new(sizeof(value_type)));
    }
};

}  // namespace xutl

#endif  // XUTL_ALLOCATOR_H