#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include <cstddef>

#include "algorithm.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"

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

    pointer _start = nullptr;                           // 已用空间的头
    pointer _finish = nullptr;                          // 已用空间的尾
    pointer _end_of_storage = nullptr;                  // 可用空间的尾
    allocator_type _data_allocator = allocator_type();  // 分配器

    // 构造函数

    vector_base() noexcept = default;

    ~vector_base() {
        _clear();
        _data_allocator.deallocate(_start, _capacity());
    }

    // 清空元素，即析构所有元素
    void _clear() noexcept {
        _destroy_at_end(_start);
    }

    // 可用空间
    size_type _capacity() const noexcept {
        return static_cast<size_type>(_end_of_storage - _start);
    }

    // 从末尾开始析构，一直达到新末尾
    void _destroy_at_end(pointer new_end) noexcept {
        while (new_end != _finish) {
            _data_allocator.destroy(--_finish);
        }
    }

    // 为 n 个对象分配空间
    void _allocate(size_type n) {
        _finish = _start = _data_allocator.allocate(n);
        _end_of_storage = _start + n;
    }

    // 回收所有空间
    void _deallocate() noexcept {
        if (_start != nullptr) {
            _clear();
            _data_allocator.deallocate(_start, _capacity());
            _start = _finish = _end_of_storage = nullptr;
        }
    }
};

// vector 类
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

private:  // 数据成员
    using base::_data_allocator;
    using base::_end_of_storage;
    using base::_finish;
    using base::_start;

public:
    // ********************************************************************************
    // 构造函数/析构函数
    // ********************************************************************************

    // 构造一个没有元素的 vector
    vector() noexcept = default;
    // 构造一个元素为默认构造的 vector
    explicit vector(size_type n) {
        if (n > 0) {
            _allocate(n);
            _construct_at_end(n);
        }
    }
    // 构造一个元素为 value 拷贝的 vector
    vector(size_type n, const_reference value) {
        if (n > 0) {
            _allocate(n);
            _construct_at_end(n, value);
        }
    }
    // 用 [first, last) 内的元素构造 vector
    template <typename InputIterator>
    vector(InputIterator first, InputIterator last,
           typename enable_if<
               xutl::is_input_iterator<InputIterator>::value &&
               !xutl::is_forward_iterator<InputIterator>::value>::type* =
               nullptr) {
        while (first != last) {
            push_back(*first);
            ++first;
        }
    }
    template <typename ForwardIterator>
    vector(ForwardIterator first, ForwardIterator last,
           typename enable_if<
               xutl::is_forward_iterator<ForwardIterator>::value>::type* =
               nullptr) {
        size_type n = static_cast<size_type>(xutl::distance(first, last));
        if (n > 0) {
            _allocate(n);
            _construct_at_end(first, last, n);
        }
    }

    // 拷贝构造函数

    vector(const vector& x) {
        size_type n = x.size();
        if (n > 0) {
            _allocate(n);
            _construct_at_end(x.begin(), x.end(), n);
        }
    }
    vector(vector&& x) noexcept {
        _start = x._start;
        _finish = x._finish;
        _end_of_storage = x._end_of_storage;
        x._start = x._finish = x._end_of_storage = nullptr;
    }

    ~vector() = default;

    // ********************************************************************************
    // 迭代器相关
    // ********************************************************************************

    iterator begin() noexcept {
        return static_cast<iterator>(_start);
    }
    iterator end() noexcept {
        return static_cast<iterator>(_finish);
    }
    const_iterator cbegin() const noexcept {
        return static_cast<const_iterator>(_start);
    }
    const_iterator cend() const noexcept {
        return static_cast<const_iterator>(_finish);
    }

    // ********************************************************************************
    // 容量相关
    // ********************************************************************************

    bool empty() const noexcept {
        return begin() == end();
    }
    size_type size() const noexcept {
        return static_cast<size_type>(end() - begin());
    }
    size_type capacity() const noexcept {
        return base::_capacity();
    }
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    // ********************************************************************************
    // 元素访问
    // ********************************************************************************

    // operator[]
    reference operator[](size_type n) {
        XUTL_DEBUG_ASSERT(n < size());
        return *(begin() + n);
    }
    reference operator[](size_type n) const {
        XUTL_DEBUG_ASSERT(n < size());
        return *(begin() + n);
    }

    // ********************************************************************************
    // 容器修改
    // ********************************************************************************

    // 析构所有元素
    void clear() noexcept {
        base::_clear();
    }

    // operator=
    vector& operator=(const vector& x);
    vector& operator=(vector&& x) noexcept;

    // assign
    template <typename InputIterator>
    typename enable_if<xutl::is_input_iterator<InputIterator>::value &&
                           !xutl::is_forward_iterator<InputIterator>::value,
                       void>::type
    assign(InputIterator first, InputIterator last);
    template <typename ForwardIterator>
    typename enable_if<xutl::is_forward_iterator<ForwardIterator>::value,
                       void>::type
    assign(ForwardIterator first, ForwardIterator last);
    void assign(size_type n, const_reference value);

    // push_back
    void push_back(const_reference x);
    void push_back(value_type&& x);

    // emplace_back
    template <typename... Args>
    void emplace_back(Args&&... args);

    // pop_back
    void pop_back();

    // swap
    void swap(vector&) noexcept;

private:
    // 分配和回收内存空间 _allocate() 和 _deallocate()
    using base::_allocate;
    using base::_deallocate;

    // 构造和析构元素 _construct_at_end() 和 _destroy_at_end()

    // 默认构造
    void _construct_at_end(size_type n) {
        while (--n) {
            _data_allocator.construct(_finish++);
        }
    }
    // 用 value 构造
    void _construct_at_end(size_type n, const_reference value) {
        while (--n) {
            _data_allocator.construct(_finish++, value);
        }
    }
    // 用迭代器范围内的值构造
    template <typename ForwardIterator>
    typename enable_if<is_forward_iterator<ForwardIterator>::value, void>::type
    _construct_at_end(ForwardIterator first, ForwardIterator last,
                      size_type n) {
        _data_allocator.construct_range_forward(first, last, end());
    }
    // _destroy_at_end()
    using base::_destroy_at_end;

    // 扩容时的建议容量
    size_type _recommend_size(size_type new_size) const {
        const size_type ms = max_size();
        if (new_size > ms) {
            THROW_LENGTH_ERROR("vector<T> is too large");
        }
        const size_type cap = capacity();
        if (cap >= ms / 2) return ms;
        return xutl::max<size_type>(2 * capacity(), new_size);
    }
};

template <typename T>
vector<T>& vector<T>::operator=(const vector<T>& x) {
    if (this != &x) {
        assign(x.begin(), x.end());
    }
    return *this;
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T>&& x) noexcept {
    _deallocate();
    _start = x._start;
    _finish = x._finish;
    _end_of_storage = x._end_of_storage;
    x._start = x._finish = x._end_of_storage = nullptr;
}

template <typename T>
template <typename InputIterator>
typename enable_if<xutl::is_input_iterator<InputIterator>::value &&
                       !xutl::is_forward_iterator<InputIterator>::value,
                   void>::type
vector<T>::assign(InputIterator first, InputIterator last) {
    clear();
    while (first != last) {
        push_back(*first);
        ++first;
    }
}

template <typename T>
template <typename ForwardIterator>
typename enable_if<xutl::is_forward_iterator<ForwardIterator>::value,
                   void>::type
vector<T>::assign(ForwardIterator first, ForwardIterator last) {
    size_type new_size = static_cast<size_type>(xutl::distance(first, last));
    if (new_size <= capacity()) {
        // if new_size <= size()
        ForwardIterator mid = last;
        bool is_growing = false;
        if (new_size > size()) {
            mid = first;
            xutl::advance(mid, size());
            is_growing = true;
        }
        pointer new_last = xutl::copy_forward(first, mid, begin());
        if (is_growing) {
            _construct_at_end(mid, last, new_size - size());
        } else {
            _destroy_at_end(new_last);
        }
    } else {
        _deallocate();
        _allocate(_recommend_size(new_size));
        _construct_at_end(first, last, new_size);
    }
}

template <typename T>
void vector<T>::assign(size_type n, const_reference value) {
    if (n <= capacity()) {
        size_type old_size = size();
        fill_forward_n(_start, xutl::min(n, old_size), value);
        if (n > old_size) {
            _construct_at_end(n - old_size, value);
        } else {
            _destroy_at_end(begin() + n);
        }
    } else {
        _deallocate();
        _allocate(_recommend_size(n));
        _construct_at_end(n, value);
    }
}

template <typename T>
void vector<T>::push_back(const_reference x) {
}

}  // namespace xutl

#endif  // XUTL_VECTOR_H_