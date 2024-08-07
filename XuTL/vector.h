#ifndef XUTL_VECTOR_H_
#define XUTL_VECTOR_H_

/**
 * 该文件包含一个模板类 vector
 */

#include <cstddef>
#include <initializer_list>

#include "algorithm.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "uninitialized.h"
#include "utils.h"

namespace xutl
{

// vector_base 类
// 按照 RAII，vector_base 管理 vector 的资源，包括管理所有数据成员，
// 负责内存空间的分配和回收，并利用该类的析构函数统一析构所有元素
template <typename T>
class vector_base
{
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

    pointer _start = nullptr;                // 已用空间的头
    pointer _finish = nullptr;               // 已用空间的尾
    pointer _end_of_storage = nullptr;       // 可用空间的尾
    using _data_allocator = allocator_type;  // 分配器

    // 构造函数

    vector_base() noexcept = default;

    ~vector_base()
    {
        _deallocate();
    }

    // 清空元素，即析构所有元素
    void _clear() noexcept
    {
        _destroy_at_end(_start);
    }

    // 可用空间
    size_type _capacity() const noexcept
    {
        return static_cast<size_type>(_end_of_storage - _start);
    }

    // 从末尾开始析构，一直达到新末尾
    void _destroy_at_end(pointer new_end) noexcept
    {
        _data_allocator::destroy(new_end, _finish);
    }

    // 为 n 个对象分配空间
    void _allocate(size_type n)
    {
        _finish = _start = _data_allocator::allocate(n);
        _end_of_storage = _start + n;
    }

    // 清空并释放所有空间
    void _deallocate() noexcept
    {
        if (_start != nullptr)
        {
            _clear();
            _data_allocator::deallocate(_start, _capacity());
            _start = _finish = _end_of_storage = nullptr;
        }
    }
};

// vector 类
template <typename T>
class vector : private vector_base<T>
{
public:
    static_assert(!std::is_same<typename std::remove_cv<T>, T>::value,
                  "xutl::vector 必须具有 non-const, non-volatile value_type");

    static_assert(!std::is_same<bool, T>::value, "xutl::vector<bool> 被禁止");

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
    using reverse_iterator = xutl::reverse_iterator<iterator>;
    using const_reverse_iterator = xutl::reverse_iterator<const_iterator>;

private:  // 数据成员
    using _data_allocator = typename base::_data_allocator;
    using base::_end_of_storage;
    using base::_finish;
    using base::_start;

public:
    // ********************************************************************************
    // 构造函数/析构函数
    // ********************************************************************************

    // 构造一个没有元素的 vector
    vector() noexcept
    {
        try
        {
            _allocate(static_cast<size_type>(16));
        }
        catch (...)
        {
            _start = _finish = _end_of_storage = nullptr;
        }
    }
    // 构造一个元素为默认构造的 vector
    explicit vector(size_type n)
    {
        if (n > 0)
        {
            _allocate(n);
            _construct_at_end(n);
        }
    }
    // 构造一个元素为 value 拷贝的 vector
    vector(size_type n, const_reference value)
    {
        if (n > 0)
        {
            _allocate(n);
            _construct_at_end(n, value);
        }
    }
    // 用 [first, last) 内的元素构造 vector
    template <typename InputIterator>
    vector(InputIterator first, InputIterator last,
           typename enable_if<
               xutl::is_input_iterator<InputIterator>::value>::type* = nullptr)
    {
        size_type n = static_cast<size_type>(xutl::distance(first, last));
        if (n > 0)
        {
            _allocate(n);
            _construct_at_end(first, last);
        }
    }

    // 拷贝构造函数

    vector(const vector& x)
    {
        size_type n = x.size();
        if (n > 0)
        {
            _allocate(n);
            _construct_at_end(x.begin(), x.end());
        }
    }
    vector(vector&& x) noexcept
    {
        _start = x._start;
        _finish = x._finish;
        _end_of_storage = x._end_of_storage;
        x._start = x._finish = x._end_of_storage = nullptr;
    }
    vector(std::initializer_list<value_type> list)
    {
        size_type n = static_cast<size_type>(list.size());
        if (n > 0)
        {
            _allocate(n);
            _construct_at_end(list.begin(), list.end());
        }
    }

    ~vector() = default;

    // ********************************************************************************
    // 迭代器相关
    // ********************************************************************************

    iterator begin() noexcept
    {
        return iterator(_start);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(_start);
    }
    iterator end() noexcept
    {
        return iterator(_finish);
    }
    const_iterator end() const noexcept
    {
        return const_iterator(_finish);
    }
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    const_iterator cbegin() const noexcept
    {
        return begin();
    }
    const_iterator cend() const noexcept
    {
        return end();
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }
    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    // ********************************************************************************
    // 容量相关
    // ********************************************************************************

    bool empty() const noexcept
    {
        return begin() == end();
    }
    size_type size() const noexcept
    {
        return static_cast<size_type>(end() - begin());
    }
    size_type capacity() const noexcept
    {
        return base::_capacity();
    }
    size_type max_size() const noexcept
    {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    void reserve(size_type n)
    {
        if (n > capacity())
        {
            _reallocate(n, false);
        }
    }

    // 丢弃多余的容量
    void shrink_to_fit()
    {
        if (_finish < _end_of_storage)
        {
            _reallocate(size(), false);
        }
    }

    // ********************************************************************************
    // 元素访问
    // ********************************************************************************

    // operator[]
    reference operator[](size_type n)
    {
        return *(begin() + n);
    }
    const_reference operator[](size_type n) const
    {
        return *(begin() + n);
    }

    // at
    reference at(size_type n)
    {
        if (n >= size())
        {
            THROW_OUT_OF_RANGE("vector");
        }
        return *(begin() + n);
    }
    const_reference at(size_type n) const
    {
        if (n >= size())
        {
            THROW_OUT_OF_RANGE("vector");
        }
        return *(begin() + n);
    }

    reference front()
    {
        return *begin();
    }
    const_reference front() const
    {
        return *begin();
    }
    reference back()
    {
        return *end();
    }
    const_reference back() const
    {
        return *end();
    }

    pointer data() noexcept
    {
        return begin();
    }
    const_pointer data() const noexcept
    {
        return begin();
    }

    // ********************************************************************************
    // 容器修改
    // ********************************************************************************

    // 析构所有元素
    void clear() noexcept
    {
        base::_clear();
    }

    // operator=
    vector& operator=(const vector& x);
    vector& operator=(vector&& x) noexcept;
    vector& operator=(std::initializer_list<value_type> list)
    {
        assign(list.begin(), list.end());
        return *this;
    }

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
    void assign(std::initializer_list<value_type> list);

    // push_back
    void push_back(const_reference value);
    void push_back(value_type&& value);

    // emplace_back
    // 在尾部就地构造元素，以避免额外的复制或移动开销
    template <typename... Args>
    void emplace_back(Args&&... args);

    // pop_back
    void pop_back();

    // emplace
    // 在 pos 处就地构造元素
    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args);

    // insert
    iterator insert(const_iterator position, const_reference value);
    iterator insert(const_iterator position, value_type&& value)
    {
        return emplace(position, xutl::move(value));
    }
    iterator insert(const_iterator position, size_type n,
                    const_reference value);

    template <typename InputIterator>
    typename enable_if<
        xutl::is_input_iterator<InputIterator>::value &&
            !xutl::is_forward_iterator<InputIterator>::value &&
            xutl::is_constructible<
                value_type,
                typename iterator_traits<InputIterator>::reference>::value,
        iterator>::type
    insert(const_iterator position, InputIterator first, InputIterator last);

    template <typename ForwardIterator>
    typename enable_if<
        xutl::is_forward_iterator<ForwardIterator>::value &&
            xutl::is_constructible<
                value_type,
                typename iterator_traits<ForwardIterator>::reference>::value,
        iterator>::type
    insert(const_iterator position, ForwardIterator first,
           ForwardIterator last);

    iterator insert(const_iterator position,
                    std::initializer_list<value_type> list)
    {
        return insert(position, list.begin(), list.end());
    }

    // swap
    void swap(vector&) noexcept;

private:
    // 分配和回收内存空间 _allocate() 和 _deallocate()
    using base::_allocate;
    using base::_deallocate;

    // helper functions

    // 在末尾默认构造 n 个元素
    void _construct_at_end(size_type n)
    {
        if (size() + n > capacity())
        {
            _reallocate(size() + n);
        }
        _finish = xutl::uninitialized_fill_n(end(), n, value_type());
    }
    // 在末尾用 value 构造 n 个元素
    void _construct_at_end(size_type n, const_reference value)
    {
        if (size() + n > capacity())
        {
            _reallocate(size() + n);
        }
        _finish = xutl::uninitialized_fill_n(end(), n, value);
    }
    // 在末尾用 [first, last) 的内容构造 n 个元素
    template <typename InputIterator>
    typename enable_if<is_input_iterator<InputIterator>::value, void>::type
    _construct_at_end(InputIterator first, InputIterator last)
    {
        size_type n = static_cast<size_type>(xutl::distance(first, last));
        if (size() + n > capacity())
        {
            _reallocate(size() + n);
        }
        _finish = xutl::uninitialized_copy(first, last, end());
    }

    // 把 [first, last) 的 n 个元素移动到末尾
    template <typename InputIterator>
    typename enable_if<is_input_iterator<InputIterator>::value, void>::type
    _move_at_end(InputIterator first, InputIterator last)
    {
        size_type n = static_cast<size_type>(xutl::distance(first, last));
        if (size() + n > capacity())
        {
            _reallocate(size() + n);
        }
        _finish = xutl::uninitialized_move(first, last, end());
    }

    // _destroy_at_end()
    using base::_destroy_at_end;

    // 扩容时的建议容量
    size_type _recommend_capacity(size_type new_capacity) const
    {
        const size_type ms = max_size();
        if (new_capacity > ms)
        {
            THROW_LENGTH_ERROR("vector<T> is too large");
        }
        const size_type cap = capacity();
        if (cap >= ms / 2) return ms;
        return xutl::max<size_type>(2 * capacity(), new_capacity);
    }

    // 重新分配空间（保留原来的元素）
    void _reallocate(size_type new_capacity, bool recommending = true)
    {
        size_type new_cap = new_capacity;
        if (recommending)
        {
            new_cap = _recommend_capacity(new_capacity);
        }
        auto old_begin = begin();
        auto old_end = end();
        auto old_size = size();
        _allocate(new_cap);
        try
        {
            _move_at_end(old_begin, old_end);
        }
        catch (...)
        {
            _deallocate();
            _start = old_begin;
            _finish = old_end;
            throw;
        }
        _data_allocator::destroy(old_begin, old_end);
        _data_allocator::deallocate(old_begin, old_size);
    }

    // 重新分配空间（保留原来的元素），并在 pos 处插入元素
    void _reallocate_and_insert(iterator pos, const_reference value)
    {
        const size_type new_cap = _recommend_capacity(capacity() + 1);
        iterator old_begin = begin();
        iterator old_end = end();
        size_type old_size = size();
        _allocate(new_cap);
        try
        {
            _move_at_end(old_begin, pos);
            _construct_at_end(1, value);
            _move_at_end(pos, old_end);
        }
        catch (...)
        {
            _deallocate();
            _start = old_begin;
            _finish = old_end;
            throw;
        }
        _data_allocator::destroy(old_begin, old_end);
        _data_allocator::deallocate(old_begin, old_size);
    }

    // 重新分配空间（保留原来的元素），并在 pos 处就地构造元素
    template <typename... Args>
    void _reallocate_and_emplace(iterator pos, Args&&... args)
    {
        const size_type new_cap = _recommend_capacity(capacity() + 1);
        auto old_begin = begin();
        auto old_end = end();
        auto old_size = size();
        _allocate(new_cap);
        try
        {
            _move_at_end(old_begin, pos);
            _data_allocator::construct(xutl::address_of(*_finish),
                                       xutl::forward<Args>(args)...);
            ++_finish;
            _move_at_end(pos, old_end);
        }
        catch (...)
        {
            _deallocate();
            _start = old_begin;
            _finish = old_end;
            throw;
        }
        _data_allocator::destroy(old_begin, old_end);
        _data_allocator::deallocate(old_begin, old_size);
    }

    // 重新分配空间（保留原来的元素），并从 pos 处开始插入 n 个元素
    void _reallocate_and_fill_n(iterator pos, size_type n,
                                const_reference value)
    {
        const size_type new_cap = _recommend_capacity(capacity() + n);
        auto old_begin = begin();
        auto old_end = end();
        auto old_size = size();
        _allocate(new_cap);
        try
        {
            _move_at_end(old_begin, pos);
            xutl::fill_n(_finish, n, value);
            _move_at_end(pos, old_end);
        }
        catch (...)
        {
            _deallocate();
            _start = old_begin;
            _finish = old_end;
            throw;
        }
        _data_allocator::destroy(old_begin, old_end);
        _data_allocator::deallocate(old_begin, old_size);
    }
};

template <typename T>
vector<T>& vector<T>::operator=(const vector<T>& x)
{
    if (this != &x)
    {
        assign(x.begin(), x.end());
    }
    return *this;
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T>&& x) noexcept
{
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
vector<T>::assign(InputIterator first, InputIterator last)
{
    clear();
    while (first != last)
    {
        push_back(*first);
        ++first;
    }
}

template <typename T>
template <typename ForwardIterator>
typename enable_if<xutl::is_forward_iterator<ForwardIterator>::value,
                   void>::type
vector<T>::assign(ForwardIterator first, ForwardIterator last)
{
    const size_type new_size =
        static_cast<size_type>(xutl::distance(first, last));
    if (new_size <= capacity())
    {
        // if new_size <= size()
        ForwardIterator mid = last;
        bool is_growing = false;
        if (new_size > size())
        {
            mid = first;
            xutl::advance(mid, size());
            is_growing = true;
        }
        pointer new_last = xutl::copy(first, mid, begin());
        if (is_growing)
        {
            _construct_at_end(mid, last);
        }
        else
        {
            _destroy_at_end(new_last);
        }
    }
    else
    {
        _deallocate();
        _allocate(_recommend_capacity(new_size));
        _construct_at_end(first, last);
    }
}

template <typename T>
void vector<T>::assign(size_type n, const_reference value)
{
    if (n <= capacity())
    {
        size_type old_size = size();
        fill_n(_start, xutl::min(n, old_size), value);
        if (n > old_size)
        {
            _construct_at_end(n - old_size, value);
        }
        else
        {
            _destroy_at_end(begin() + n);
        }
    }
    else
    {
        _deallocate();
        _allocate(_recommend_capacity(n));
        _construct_at_end(n, value);
    }
}

template <typename T>
void vector<T>::assign(std::initializer_list<value_type> list)
{
    assign(list.begin(), list.end());
}

template <typename T>
void vector<T>::push_back(const_reference value)
{
    if (_finish != _end_of_storage)
    {
        _data_allocator::construct(address_of(*_finish), value);
        ++_finish;
    }
    else
    {
        _reallocate_and_insert(_finish, value);
    }
}

template <typename T>
void vector<T>::push_back(value_type&& value)
{
    emplace_back(xutl::move(value));
}

template <typename T>
template <typename... Args>
void vector<T>::emplace_back(Args&&... args)
{
    if (_finish != _end_of_storage)
    {
        _data_allocator::construct(address_of(*_finish),
                                   xutl::forward<Args>(args)...);
        ++_finish;
    }
    else
    {
        _reallocate_and_emplace(_finish, xutl::forward<Args>(args)...);
    }
}

template <typename T>
void vector<T>::pop_back()
{
    if (!empty())
    {
        _destroy_at_end(_finish - 1);
        --_finish;
    }
}

template <typename T>
template <typename... Args>
typename vector<T>::iterator vector<T>::emplace(const_iterator position,
                                                Args&&... args)
{
    iterator pos = const_cast<iterator>(position);
    const size_type n = pos - _start;
    if (_finish != _end_of_storage)
    {
        if (pos == _finish)
        {
            _data_allocator::construct(xutl::address_of(*_finish),
                                       xutl::forward<Args>(args)...);
            ++_finish;
        }
        else
        {
            auto old_finish = _finish;
            _data_allocator::construct(xutl::address_of(*_finish),
                                       *(_finish - 1));
            ++_finish;
            xutl::move_backward(pos, old_finish - 1, old_finish);
            *pos = value_type(xutl::forward<Args>(args)...);
        }
    }
    else
    {
        _reallocate_and_emplace(pos, xutl::forward<Args>(args)...);
    }
    return _start + n;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator position,
                                               const_reference value)
{
    // 去除 const
    // iterator pos = const_cast<iterator>(position);
    iterator pos = _start + (position - begin());
    const size_type n = pos - _start;
    if (_finish != _end_of_storage)
    {
        if (pos == _finish)
        {
            _data_allocator::construct(xutl::address_of(*_finish), value);
            ++_finish;
        }
        else
        {
            auto old_finish = _finish;
            _data_allocator::construct(xutl::address_of(*_finish),
                                       *(_finish - 1));
            ++_finish;
            // value 是引用，如果它是内部元素之一，那么它可能被改变，因此先拷贝
            auto value_copy = value;
            xutl::move_backward(pos, old_finish - 1, old_finish);
            *pos = xutl::move(value_copy);
        }
    }
    else
    {
        _reallocate_and_insert(pos, value);
    }
    return _start + n;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator position,
                                               size_type n,
                                               const_reference value)
{
    iterator pos = _start + (position - begin());
    if (n <= 0) return pos;
    // 如果插入 n 后，容量不会超过上限
    if (n <= static_cast<size_type>(_end_of_storage - end()))
    {
        auto old_n = n;
        auto old_end = _finish;
        // 如果插入的 n 个元素比原来的末尾还要长，先插入末尾多出来的元素
        if (n > static_cast<size_type>(end() - pos))
        {
            size_type n_at_end = n - (end() - pos);
            _construct_at_end(n_at_end, value);
            n -= n_at_end;
        }
        // 接着把 pos 到末尾的元素插入
        // 由于 pos 可能就是末尾，所以再判断一次 n > 0
        if (n > 0)
        {
            auto value_copy = value;
            _move_at_end(pos, old_end);
            xutl::fill_n(pos, n, value_copy);
        }
    }
    else
    {
        _reallocate_and_fill_n(pos, n, value);
    }
    return _start + n;
}

template <typename T>
void vector<T>::swap(vector<T>& rhs) noexcept
{
    if (this != &rhs)
    {
        xutl::swap(_start, rhs._start);
        xutl::swap(_finish, rhs._finish);
        xutl::swap(_end_of_storage, rhs._end_of_storage);
    }
}

}  // namespace xutl

#endif  // XUTL_VECTOR_H_