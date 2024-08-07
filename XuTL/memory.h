#ifndef XUTL_MEMORY_H_
#define XUTL_MEMORY_H_

/**
 * 该文件包含内存管理相关的一切
 * 包括模板类 allocator 作为默认分配器
 */

#include <cstring>

#include "construct.h"
#include "type_traits.h"
#include "utils.h"

namespace xutl {

// ************************************************************************************
// allocator_traits / helper classes
// ************************************************************************************

// allocator_has_construct
// allocator 是否可以构造其实参类型

// decltype 中利用逗号表达式，实际上返回值就是 true_type，前者仅用于 SFINAE
template <typename Alloc, typename T, typename... Args>
decltype(xutl::declval<Alloc>().construct(xutl::declval<T*>(),
                                          xutl::declval<Args>()...),
         true_type())
_allocator_has_construct_test(Alloc&& alloc, T* ptr, Args&&... args);

template <typename Alloc, typename T, typename... Args>
false_type _allocator_has_construct_test(const Alloc& a, T* ptr,
                                         Args&&... args);

template <typename Alloc, typename Pointer, class... Args>
struct allocator_has_construct
    : integral_constant<bool,
                        xutl::is_same<decltype(_allocator_has_construct_test(
                                          declval<Alloc>(), declval<Pointer>(),
                                          declval<Args>()...)),
                                      true_type>::value> {};

// ************************************************************************************
// allocator 类
// ************************************************************************************

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
    using allocator_type = allocator<T>;

    // 构造函数
    explicit allocator() noexcept = default;
    // 拷贝构造函数
    explicit allocator(const allocator&) noexcept = default;
    // 泛化的拷贝构造函数
    template <typename U>
    explicit allocator(const allocator<U>&) noexcept {
    }
    // 析构函数
    ~allocator() = default;

    // 把 allocator<T> 转换到 allocator<U>
    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    // 取址

    static pointer address(reference x) {
        return &x;
    }
    static const_pointer address(const_reference x) {
        return &x;
    }

    // 分配空间
    // ::operator new 返回一个 void*, 利用 static_cast 将 void* 转换成 T*

    // 分配一个大小为 sizeof(T) 的空间
    static pointer allocate() {
        return static_cast<pointer>(::operator new(sizeof(value_type)));
    }
    // 分配 n 个大小为 sizeof(T) 的空间
    static pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }

    // 释放空间

    static void deallocate(T* ptr) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }
    static void deallocate(T* ptr, size_type n) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    // 构造对象

    static void construct(T* ptr) {
        xutl::construct(ptr);
    }
    template <typename... Args>
    static void construct(T* ptr, Args&&... args) {
        xutl::construct(ptr, xutl::forward<Args>(args)...);
    }

    // 用 [begin1, end1) 的元素构造以 begin2 为起点的空间
    template <typename Iterator, typename Pointer>
    static void construct_range_forward(Iterator begin1, Iterator end1,
                                        Pointer begin2) {
        while (begin1 != end1) {
            xutl::construct(address_of(*begin2), *begin1);
            ++begin1;
            ++begin2;
        }
    }
    // 对于无构造函数类型的 SFINAE
    template <typename U>
    static typename enable_if<
        !allocator_has_construct<allocator_type, U*, U>::value &&
            xutl::is_trivially_move_constructible<U>::value,
        void>::type
    construct_range_forward(U* begin1, U* end1, U*& begin2) {
        using V = typename xutl::remove_cv<T>::type;
        difference_type n = end1 - begin1;
        if (n > 0) {
            memcpy(const_cast<V>(begin2), begin1, n * sizeof(T));
            begin2 += n;
        }
    }

    // 析构对象

    static void destroy(T* ptr) {
        xutl::destroy(ptr);
    }
    static void destroy(T* first, T* last) {
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

}  // namespace xutl

#endif  // XUTL_MEMORY_H_