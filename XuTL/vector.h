#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include <cstddef>
#include <type_traits>
#include <utility>

#include "allocator.h"
#include "debugdef.h"
#include "iterator.h"

namespace xutl {

template <typename T>
class vector {
    static_assert(std::is_same<typename std::remove_cv<T>, T>::value,
                  "xutl::vector 必须具有 non-const, non-volatile value_type");

    static_assert(std::is_same<bool, T>::value, "xutl::vector<bool> 被禁止");

public:  // vector 的嵌套类型
    // 本项目所有容器统一使用 allocator<T>，不允许自定义分配器
    using allocator_type = allocator<T>;

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    // 迭代器

    using iterator = value_type*;  // 普通指针足以作为 vector 的迭代器
    using const_iterator = const value_type*;

protected:  // 数据成员
    // 本项目的 allocator 是无状态分配器，其实这项没必要，不过为了符合标准还是加上
    using data_allocator = allocator<T>;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    // helper function: 一些 vector 的工具函数

    void default_initialize(size_type n) {
    }

    void fill_initialize(size_type n, const T& value) {
    }

    // 分配空间并填满
    iterator allocate_and_fill(size_type n, const T& value) {
    }

public:
    // 构造函数

    // 创建一个没有元素的 vector
    vector() = default;
    vector(const allocator_type& alloc) noexcept : Base(alloc) {
    }
    // 创建一个元素为默认构造的 vector.
    explicit vector(size_type n, const allocator_type& alloc = allocator_type()) : Base(n, alloc) {
        default_initialize(n);
    }
    // 创建一个元素为 value 拷贝的 vector.
    vector(size_type n, const value_type& value, const allocator_type& alloc = allocator_type())
        : Base(n, alloc) {
        fill_initialize(n, value);
    }

    // 拷贝构造函数

    vector(const vector& x) : Base(x.size(), x.get_allocator()) {
    }

    // 迭代器相关

    iterator begin() noexcept {
        return static_cast<iterator>(this->impl.start);
    }
    iterator end() noexcept {
        return static_cast<iterator>(this->impl.finish);
    }
    const_iterator cbegin() const noexcept {
        return static_cast<const_iterator>(this->impl.start);
    }
    const_iterator cend() const noexcept {
        return static_cast<const_iterator>(this->impl.finish);
    }

    // 容量相关

    bool empty() const noexcept {
        return begin() == end();
    }
    size_type size() const noexcept {
        return static_cast<size_type>(end() - begin());
    }
    size_type capacity() const noexcept {
        return static_cast<size_type>(this->impl.end_of_storage - begin());
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