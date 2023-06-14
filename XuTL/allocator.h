#ifndef XUTL_ALLOCATOR_H_
#define XUTL_ALLOCATOR_H_

/**
 * 该文件包含一个模板类 allocator 作为默认分配器.
 * allocator<T>: 用于分配和释放内存，构造和析构对象.
 * 该默认分配器是无状态的.
 */

#include <cstddef>
#include <new>

#include "construct.h"
#include "utils.h"

namespace xutl {

template <typename T>
class allocator {
public:
    // allocator 的嵌套类型

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    // 构造函数
    inline explicit allocator() noexcept = default;
    // 拷贝构造函数
    inline explicit allocator(const allocator&) noexcept = default;
    // 泛化的拷贝构造函数
    template <typename U>
    inline explicit allocator(const allocator<U>&) noexcept {
    }
    // 析构函数
    inline ~allocator() = default;

    // 把 allocator<T> 转换到 allocator<U>
    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    // 取址

    pointer address(reference x) const {
        return &x;
    }
    const_pointer address(const_reference x) const {
        return &x;
    }

    // 分配空间
    // ::operator new 返回一个 void*, 利用 static_cast 将 void* 转换成 T*

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
    void deallocate(T* ptr, size_type n) {
        if (ptr == nullptr) return;
        while (n-- && ptr != nullptr) {
            auto now = ptr++;
            ::operator delete(now);
        }
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

// 比较操作

template <typename T1, typename T2>
inline bool operator==(const allocator<T1>&, const allocator<T2>&) {
    return true;
}
template <typename T1, typename T2>
inline bool operator!=(const allocator<T1>&, const allocator<T2>&) {
    return false;
}

// allocator_traits

template <typename Alloc>
struct allocator_traits {
    template <typename U>
    struct rebind {
        using other = typename Alloc::template rebind<U>::other;
    };

    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;
};

}  // namespace xutl

#endif  // XUTL_ALLOCATOR_H_