#ifndef XUTL_ALLOCATOR_H_
#define XUTL_ALLOCATOR_H_

/**
 * 该文件包含一个模板类 allocator
 * allcator<T>: 用于分配和释放内存，构造和析构对象
 */

#include <cstddef>
#include <new>

namespace xutl {

template <typename T>
class Allocator {
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
    inline explicit Allocator() = default;

    // 复制构造函数
    inline explicit Allocator(const Allocator&) = default;

    // 泛化的复制构造函数
    template <typename U>
    inline explicit Allocator(const Allocator<U>&) {
    }

    // 析构函数
    inline ~Allocator() = default;

    // 把 allocator<T> 转换到 allocator<U>
    template <typename U>
    struct rebind {
        using other = Allocator<U>;
    };

    // ::operator new 分配一块空间，大小为 sizeof(T)，返回一个 void*
    // 利用 static_cast 将 void* 转换成 T*
    pointer allocate() {
        return static_cast<pointer>(::operator new(sizeof(value_type)));
    }

    // 分配 n 个 sizeof(T) 大小的空间
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }

    // 释放空间
    void deallocate(pointer ptr) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    void deallocate(pointer ptr, size_type) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    void construct(pointer ptr) {
    }
};

}  // namespace xutl

#endif  // XUTL_ALLOCATOR_H_