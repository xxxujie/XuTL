#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include <cstddef>
#include <utility>

#include "allocator.h"
#include "debugdef.h"

namespace xutl {

// vector_base 遵循 RAII 思想，统一管理 vector 的资源, 包括:
// 1. vector 的数据成员: start, finish, end_of_storage 以及 vector_impl.
// 2. T 对象的内存. 也就是说，该类负责 T 的内存分配和回收, 而 vector 本身专注于 T 的构造和析构.
//
// 另外, 这种做法更容易实现异常安全. vector 的构造函数在构造过程中, 只要其中一个对象抛出异常,
// 就需要销毁所有已经构造好的对象. 有了 vector_base, 就不需要每个 vector 构造函数单独处理, 而是由
// vector_base 的析构函数统一处理.
template <typename T, typename Alloc>
struct vector_base {
public:  // 声明 vector_impl
    // Alloc<T>
    using T_allocator_type = typename allocator_traits<Alloc>::template rebind<T>::other;
    // 指向实际 Alloc<?> 中 ? 的指针
    using pointer = typename allocator_traits<Alloc>::pointer;

    // vector 的数据成员一并放在 vector_impl 中
    struct vector_impl_data {
        pointer start;
        pointer finish;
        pointer end_of_storage;

        vector_impl_data() noexcept : start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        }
        vector_impl_data(vector_impl_data&& x) noexcept
            : start(x.start), finish(x.finish), end_of_storage(x.end_of_storage) {
        }
    };

    // 有状态 allocator 必须要声明为 vector 的数据成员以在构造函数中初始化.
    // vector_impl 继承 allocator 以利用空基类优化 (EBO), 这样, 空 allocator 类
    // 不会占据额外的内存空间.
    //
    // 由于容器必须通过 traits 获取分配器相关信息, 所以 _Vector_impl 只有能够扮演 allocator
    // 的角色, 容器才能从中萃取信息. 也就是说, vector_impl 必须要 public 继承 allocator,
    // 建立 is-a 的关系.
    struct vector_impl : public T_allocator_type, public vector_impl_data {
        vector_impl() noexcept : T_allocator_type() {
        }
        vector_impl(const T_allocator_type& alloc) : T_allocator_type(alloc) {
        }
        vector_impl(vector_impl&& x) noexcept
            : T_allocator_type(std::move(x)), vector_impl_data(std::move(x)) {
        }
        vector_impl(T_allocator_type&& alloc) noexcept : T_allocator_type(std::move(alloc)) {
        }
        vector_impl(T_allocator_type&& alloc, vector_impl rv) noexcept
            : T_allocator_type(std::move(alloc)), vector_impl(std::move(rv)) {
        }
    };

public:
    // Alloc<?>
    using allocator_type = Alloc;

    T_allocator_type& get_T_allocator() noexcept {
        return this->impl;
    }
    const T_allocator_type& get_T_allocator() const noexcept {
        return this->impl;
    }

    allocator_type get_allocator() const noexcept {
        return static_cast<allocator_type>(get_T_allocator());
    }

    // 构造函数
    vector_base() = default;
    vector_base(const allocator_type& alloc) noexcept : impl(alloc) {
    }
    vector_base(size_t n) : impl() {
        create_storage(n);
    }
    vector_base(size_t n, const allocator_type& alloc) : impl(alloc) {
        create_storage(n);
    }
    vector_base(allocator_type&&) = default;
    vector_base(T_allocator_type&& alloc) noexcept : impl(std::move(alloc)) {
    }
    vector_base(vector_base&& x, const allocator_type& alloc) : impl(alloc) {
        if (x.get_allocator() == alloc) {}
    }

public:
    vector_impl impl;

protected:
    void create_storage(size_t n);
};

template <typename T, typename Alloc = allocator<T>>
class vector {
public:
    // vector 的嵌套类型

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;  // 普通指针足以作为 vector 的迭代器
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

    // 分配空间并填满
    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = Alloc::allocate(n);
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