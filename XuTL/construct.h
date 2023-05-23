#ifndef XUTL_CONSTRUCT_H_
#define XUTL_CONSTRUCT_H_

/**
 * 该文件包含两个工具函数，给 allcator 使用，包括 construct 和 destroy
 */

#include <new>
#include <type_traits>

#include "iterator.h"
#include "utils.h"

namespace xutl {

// construct: 用于构造对象
// 三个版本

// 空参构造
template <typename T>
inline void construct(T* ptr) {
    ::new (ptr) T();
}

// 单个参数
template <typename T1, typename T2>
inline void construct(T1* ptr, const T2& value) {
    ::new (ptr) T1(value);
}

// 万能引用参数包, 单个 rvalue 会匹配这个
template <typename T, typename... Args>
inline void construct(T* ptr, Args&&... value_packet) {
    ::new (ptr) T(xutl::forward<Args>(value_packet)...);
}

// destructor: 用于析构对象
// 两个版本: 1. 接受单个指针 2. 接受两个迭代器
// 多次调用 trivial destructor 会浪费性能，尤其是版本 2 的范围可能很大，因此要判断类型 T 的
// destructor 是否 trivial;

// 版本 1
template <typename T>
inline void destroy(T* ptr) {
    // 判断是否有 trivial destructor
    _destroy_pointer(ptr, std::is_trivially_destructible<T>::type);
}

// trivial destructor
template <typename T>
inline void _destroy_pointer(T* ptr, std::true_type) {  // 什么都不做
}

// nontrivial destructor
template <typename T>
inline void _destroy_pointer(T* ptr, std::false_type) {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

// 版本 2
template <typename ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) {
    _destroy_iterator(first, last,
                      std::is_trivially_destructible<
                          typename iterator_traits<ForwardIterator>::value_type>::type);
}

// trivial destructor
template <typename ForwardIterator>
void _destroy_iterator(ForwardIterator first, ForwardIterator last, std::true_type) {
}

// nontrivial destructor
template <typename ForwardIterator>
void _destroy_iterator(ForwardIterator first, ForwardIterator last, std::false_type) {
    while (first++ != last) {
        // auto current = &*first;
        destroy(&*first);
    }
}

}  // namespace xutl

#endif  // XUTL_CONSTRUCT_H_