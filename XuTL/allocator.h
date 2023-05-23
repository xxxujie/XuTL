#ifndef XUTL_ALLOCATOR_H_
#define XUTL_ALLOCATOR_H_

/**
 * 该文件包含一个模板类 allocator
 * allcator<T>: 用于分配和释放内存，构造和析构对象
 */

#include <cstddef>
#include <new>

#include "construct.h"
#include "utils.h"

namespace xutl {

template <typename T>
class allocator {
public:
    // type T 本身
    using value_type = T;
    // pointer to T
    using pointer = T*;
    // pointer to constant T
    using const_pointer = const T*;
    // reference to T
    using reference = T&;
    // reference to constant T
    using const_reference = const T&;
    // (unsigned int) size of the largest object in the allocation model
    using size_type = size_t;
    // (int) difference between any two pointers in the allocation model
    using difference_type = std::ptrdiff_t;

    // 构造函数
    inline explicit allocator() = default;

    // 复制构造函数
    inline explicit allocator(const allocator&) = default;

    // 泛化的复制构造函数
    template <typename U>
    inline explicit allocator(const allocator<U>&) {
    }

    // 析构函数
    inline ~allocator() = default;

    // 把 allocator<T> 转换到 allocator<U>
    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    // ::operator new 分配一块空间, 大小为 sizeof(T), 返回一个 void*,
    // 并利用 static_cast 将 void* 转换成 T*

    // 分配一个大小为 sizeof(T) 的空间
    pointer allocate() {
        return static_cast<pointer>(::operator new(sizeof(value_type)));
    }

    // 分配 n 个大小为 sizeof(T) 的空间
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }

    // 释放空间

    void deallocate(T* ptr) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    void deallocate(T* ptr, size_type) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    // 构造对象

    void construct(T* ptr) {
        xutl::construct(ptr);
    }

    void construct(T* ptr, const T& value) {
        xutl::construct(ptr, value);
    }

    void construct(T* ptr, T&& value) {
        xutl::construct(ptr, value);
    }

    template <typename... Args>
    void construct(T* ptr, Args&&... args) {
        xutl::construct(ptr, xutl::forward<Args>(args)...);
    }

    // 析构对象

    void destroy(T* ptr) {
        xutl::destroy(ptr);
    }

    void destroy(T* first, T* last) {
        xutl::destroy(first, last);
    }
};

}  // namespace xutl

#endif  // XUTL_ALLOCATOR_H_