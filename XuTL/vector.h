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
    // Alloc<T>, 我在 vector 中规定 Alloc 的 value_type 必须和 T 相同
    using allocator_type = typename allocator_traits<Alloc>::template rebind<T>::other;
    // T*
    using pointer = typename allocator_traits<Alloc>::pointer;

    // vector 的数据成员一并放在 vector_impl 中
    struct vector_impl_data {
        pointer start;           // 当前使用空间的头
        pointer finish;          // 当前使用空间的尾
        pointer end_of_storage;  // 当前可用空间的尾

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
    struct vector_impl : public allocator_type, public vector_impl_data {
        vector_impl() noexcept : allocator_type() {
        }
        vector_impl(const allocator_type& alloc) : allocator_type(alloc) {
        }
        vector_impl(vector_impl&& x) noexcept
            : allocator_type(std::move(x)), vector_impl_data(std::move(x)) {
        }
        vector_impl(allocator_type&& alloc) noexcept : allocator_type(std::move(alloc)) {
        }
        vector_impl(allocator_type&& alloc, vector_impl rv) noexcept
            : allocator_type(std::move(alloc)), vector_impl(std::move(rv)) {
        }
    };

public:
    allocator_type get_allocator() {
        return this->impl;
    }

    const allocator_type get_allocator() const {
        return this->impl;
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
    vector_base(allocator_type&& alloc) noexcept : impl(std::move(alloc)) {
    }

    // 析构函数
    ~vector_base() noexcept {
        deallocate(impl.start, impl.end_of_storage - impl.start);
    }

public:
    vector_impl impl;

    pointer allocate(size_t n) {
        if (n != 0) {
            return allocator_type::allocate(n);
        } else {
            return pointer();
        }
    }

    void deallocate(pointer ptr, size_t n) {
        allocator_type::deallocate(ptr, n);
    }

protected:
    void create_storage(size_t n) {
        this->impl.start = this->allocate(n);
        this->impl.finish = this->impl.start;
        this->impl.end_of_storage = this->impl.start + n;
    }
};

template <typename T, typename Alloc = allocator<T>>
class vector : protected vector_base<T, Alloc> {
    static_assert(std::is_same<typename std::remove_cv<T>, T>::value,
                  "xutl::vector 必须具有 non-const, non-volatile value_type");
    static_assert(std::is_same<typename Alloc::value_type, T>::value,
                  "xutl::vector 必须与其 allocator 具有相同的 value_type");

    // vector 的嵌套类型

    using Base = vector_base<T, Alloc>;

public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = value_type*;  // 普通指针足以作为 vector 的迭代器
    using const_iterator = const value_type*;

protected:  // 数据成员
    using Base::allocate;
    using Base::deallocate;
    using Base::impl;

    // helper function: 一些 vector 的工具函数

    void default_initialize(size_type n) {
    }

    void fill_initialize(size_type n, const T& value) {
    }

    // 分配空间并填满
    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = Alloc::allocate(n);
    }

public:
    // 构造函数

    vector() = default;
    vector(const allocator_type& alloc) noexcept : Base(alloc) {
    }
    explicit vector(size_type n, const allocator_type& alloc = allocator_type()) : Base(n, alloc) {
        default_initialize(n);
    }
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