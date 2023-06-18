#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include "allocator.h"
#include "debugdef.h"
#include "iterator.h"

namespace xutl {

// vector_base 类
// 按照 RAII，vector_base 管理 vector 的资源，包括管理所有数据成员，
// 负责内存空间的分配和回收，并利用该类的析构函数统一析构所有元素
template <typename T>
class vector_base {
protected:
    // 本项目所有容器统一使用 allocator<T>，不允许自定义分配器
    using allocator_type = allocator<T>;

    // vector 的嵌套类型
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    // 迭代器
    using iterator = pointer;  // 普通指针足以作为 vector 的迭代器
    using const_iterator = const_pointer;

    // 数据成员

    pointer start = nullptr;                           // 已用空间的头
    pointer finish = nullptr;                          // 已用空间的尾
    pointer end_of_storage = nullptr;                  // 可用空间的尾
    allocator_type data_allocator = allocator_type();  // 分配器

    // 构造函数

    vector_base() noexcept = default;
    ~vector_base() {
        clear();
        data_allocator.deallocate(start, capacity());
    }

    // 清空元素，即析构所有元素
    inline void clear() noexcept {
        destroy_at_end(start);
    }

    // 可用空间
    inline size_type capacity() const noexcept {
        return static_cast<size_type>(end_of_storage - start);
    }

    // 从末尾开始析构，一直达到新末尾
    inline void destroy_at_end(pointer new_end) noexcept {
        while (new_end != finish) {
            data_allocator.destroy(--finish);
        }
    }

    // 为 n 个对象分配空间
    void allocate(size_type n) {
        finish = start = data_allocator.allocate(n);
        end_of_storage = start + n;
    }

    // 回收所有空间
    void deallocate() noexcept {
        if (start != nullptr) {
            clear();
            data_allocator.deallocate(start, capacity());
            start = finish = end_of_storage = nullptr;
        }
    }
};

template <typename T>
class vector : private vector_base<T> {
public:
    static_assert(std::is_same<typename std::remove_cv<T>, T>::value,
                  "xutl::vector 必须具有 non-const, non-volatile value_type");

    static_assert(std::is_same<bool, T>::value, "xutl::vector<bool> 被禁止");

private:
    using base = vector_base<T>;

public:
    using allocator_type = allocator<T>;

    using value_type = T;
    using reference = typename base::reference;
    using const_reference = typename base::const_reference;
    using pointer = typename base::pointer;
    using const_pointer = typename base::const_pointer;
    using size_type = typename base::size_type;
    using difference_type = typename base::difference_type;

    // 迭代器
    using iterator = value_type*;
    using const_iterator = const value_type*;

private:
    // 数据成员
    using base::data_allocator;
    using base::end_of_storage;
    using base::finish;
    using base::start;

    // 分配和回收内存空间 allocate() 和 deallocate()
    using base::allocate;
    using base::deallocate;

    // 构造和析构元素 construct_at_end() 和 destroy_at_end()

    // 默认构造
    void construct_at_end(size_type n) {
        while (--n) {
            data_allocator.construct(finish++);
        }
    }
    // 用 value 构造
    void construct_at_end(size_type n, const_reference value) {
        while (--n) {
            data_allocator.construct(finish++, value);
        }
    }
    // 在迭代器范围内构造

    // 填满，即分配空间并构造，用于构造函数
    void fill_init(size_type n) {
        allocate(n);
        construct_at_end(n);
    }
    void fill_init(size_type n, const_reference value) {
        allocate(n);
        construct_at_end(n, value);
    }
    void fill_init() {
    }

public:
    // 构造函数

    // 创建一个没有元素的 vector
    inline vector() = default;
    // 创建一个元素为默认构造的 vector.
    explicit vector(size_type n) {
        if (n > 0) {
            fill_init(n);
        }
    }
    // 创建一个元素为 value 拷贝的 vector.
    vector(size_type n, const_reference value) {
        if (n > 0) {
            fill_init(n, value);
        }
    }

    // 拷贝构造函数

    vector(const vector& x) {
        size_type n = x.size();
        if (n > 0) {}
    }

    // 迭代器相关

    inline iterator begin() noexcept {
        return static_cast<iterator>(start);
    }
    inline iterator end() noexcept {
        return static_cast<iterator>(finish);
    }
    inline const_iterator cbegin() const noexcept {
        return static_cast<const_iterator>(start);
    }
    inline const_iterator cend() const noexcept {
        return static_cast<const_iterator>(finish);
    }

    // 容量相关

    inline bool empty() const noexcept {
        return begin() == end();
    }
    inline size_type size() const noexcept {
        return static_cast<size_type>(end() - begin());
    }

    inline size_type capacity() const noexcept {
        return base::capacity();
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