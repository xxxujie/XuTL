#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include <cstddef>

#include "allocator.h"
#include "debugdef.h"

namespace xutl {

template <typename T, typename Alloc = allocator<T>>
class vector {
public:
    // vector 的嵌套类型

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

protected:
    iterator start;           // 当前使用空间的头
    iterator finish;          // 当前使用空间的尾
    iterator end_of_storage;  // 当前可用空间的尾

    // helper function: 一些 vector 的工具函数

    void fill_initialize(size_type n, const T& value) {
    }

    iterator allocate_and_fill(size_type new_size, const T& x) {
    }

public:
    // 构造、拷贝、移动、析构函数

    vector();

    // 迭代器相关

    iterator begin() noexcept {
        return start;
    }
    iterator end() noexcept {
        return finish;
    }
    const_iterator cbegin() const noexcept {
        return start;
    }
    const_iterator cend() const noexcept {
        return finish;
    }

    // 容量相关

    bool empty() const noexcept {
        return begin() == end();
    }
    size_type size() const noexcept {
        return static_cast<size_type>(end() - begin());
    }
    size_type capacity() const noexcept {
        return static_cast<size_type>(end_of_storage - begin());
    }

    // 元素访问

    reference operator[](size_type n) {
        XUTL_DEBUG_ASSERT(n < size());
        return *(begin() + n);
    }
    reference operator[](size_type n) const {
        XUTL_DEBUG_ASSERT(n < size());
        return *(begin() + n);
    }
};

}  // namespace xutl

#endif  // XUTL_VECTOR_H_