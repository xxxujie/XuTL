#ifndef XUTL_LIST_H_
#define XUTL_LIST_H_

/**
 * 该文件包含一个模板类 list
 * 它是一个双向链表
 */

#include <initializer_list>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "utils.h"

namespace xutl {

template <class T>
struct list_node;

template <class T>
struct node_traits {
    using node_ptr = list_node<T>*;
};

// list_node 类
template <class T>
struct list_node {
    using node_ptr = typename node_traits<T>::node_ptr;

    node_ptr prev;  // 前一节点
    node_ptr next;  // 后一节点
    T data;         // 数据域
};

// list 的迭代器
template <class T>
struct list_iterator
    : public xutl::iterator<xutl::bidirectional_iterator_tag, T> {
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using node_ptr = typename node_traits<T>::node_ptr;
    using self = list_iterator<T>;

    // 指向 list 的节点
    node_ptr node;

    // 构造函数
    list_iterator() = default;
    list_iterator(node_ptr x) : node(x) {
    }
    list_iterator(const list_iterator& rhs) : node(rhs.node) {
    }

    bool operator==(const self& rhs) const {
        return node == rhs.node;
    }
    bool operator!=(const self& rhs) const {
        return node != rhs.node;
    }

    // 重载符号
    reference operator*() const {
        return node->data;
    }
    pointer operator->() const {
        return &(operator*());
    }

    // 前置 ++
    self& operator++() {
        node = node->next;
        return *this;
    }
    // 后置 ++
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    // 前置 -- 和后置 --
    self& operator--() {
        node = node->prev;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template <class T>
class list {
public:
    using allocator_type = xutl::allocator<T>;
    using data_allocator = xutl::allocator<T>;
    using node_allocator = xutl::allocator<list_node<T>>;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = list_iterator<T>;
    using const_iterator = list_iterator<T>;
    using reverse_iterator = xutl::reverse_iterator<iterator>;

    using node_ptr = typename node_traits<T>::node_ptr;

private:
    // node 是尾端的一个空白节点，整个 list 是一个环状双向链表
    node_ptr _node;

public:
    // 构造函数
    list() {
        _fill_init(0, value_type());
    }

    explicit list(size_type n) {
        _fill_init(n, value_type());
    }

    list(size_type n, const_reference value) {
        _fill_init(n, value);
    }

    template <class Iterator,
              typename xutl::enable_if<xutl::is_input_iterator<iterator>::value,
                                       int>::type* = 0>
    list(Iterator first, Iterator last) {
        _copy_init(first, last);
    }

    list(const list& rhs) {
        _copy_init(rhs.cbegin(), rhs.cend());
    }

    list(list&& rhs) noexcept : _node(rhs._node) {
        rhs._node = nullptr;
    }

public:
    // STL 通常都是「前闭后开」的区间
    iterator begin() {
        return _node->next;
    }
    iterator end() {
        return _node;
    }
    const_iterator cbegin() const {
        return _node->next;
    }
    const_iterator cend() const {
        return _node;
    }

    bool empty() const {
        // 如果整个链表只有一个尾节点，那么链表是空的
        return _node->next == _node;
    }
    size_type size() const {
        return xutl::distance(cbegin(), cend());
    }

    reference front() {
        return *begin();
    }
    reference front() const {
        return *begin();
    }
    reference back() {
        return *(--end());
    }
    reference back() const {
        return *(--end());
    }

    // 在 position 位置插入值为 value 的节点
    iterator insert(iterator position, const_reference value) {
        node_ptr tmp_node = _create_node(value);
        // 插入 tmp_node
        tmp_node->next = position.node;
        tmp_node->prev = position.node->prev;
        position.node->prev->next = tmp_node;
        position.node->prev = tmp_node;
        return tmp_node;
    }

    void push_front(const_reference value) {
        insert(begin(), value);
    }

    void push_back(const_reference value) {
        insert(end(), value);
    }

    iterator erase(iterator position) {
        node_ptr next_node = position.node->next;
        node_ptr prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        _destroy_node(position.node);
        return next_node;
    }

    void pop_front() {
        erase(begin());
    }

    void pop_back() {
        erase(end());
    }

    void clear() {
        node_ptr cur_node = begin();
        while (cur_node != _node) {
            node_ptr tmp_node = cur_node;
            _destroy_node(tmp_node);
            cur_node = cur_node->next;
        }
        // 还原 node 空链表状态
        _node->prev = _node->next = _node;
    }

    void remove(const_reference value) {
        iterator first = begin();
        iterator last = end();
        // 遍历所有节点，找到值为 value 的就移除
        while (first != last) {
            iterator next = first;
            ++next;
            if (*first == value) {
                erase(first);
            }
            first = next;
        }
    }

    // 移除连续的相同元素，使得它们只保留一个
    void unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;  // 空链表，什么都不做
        iterator next = first;
        while (++next != last) {
            if (*first == *next) {
                erase(next);
            } else {
                first = next;
            }
            next = first;
        }
    }

    // 将链表 another 接合到 postion 之前
    // x 必须不同于 *this
    void splice(iterator position, list& x) {
        if (!x.empty()) {
            _transfer(position, x.begin(), x.end());
        }
    }
    // 将 iter 所指元素接合到 position 之前
    // position 和 iter 可以指向同一个链表
    void splice(iterator position, list& x, iterator iter) {
        iterator next_iter = iter;
        ++next_iter;
        if (position == iter || position == next_iter) return;
        _transfer(position, iter, next_iter);
    }
    // 将 [first, last) 区间内的元素接合到 position 之前
    // position 和 [first, last) 可以指向同一个链表，
    // 但 position 不能位于 [first, last) 之内
    void splice(iterator position, list& x, iterator first, iterator last) {
        if (first != last) {
            _transfer(position, first, last);
        }
    }

    // 与另一个链表 x 合并
    // *this 和 x 都必须是排序过后的链表，
    // 默认为递增
    void merge(list& x) {
        merge(x, xutl::less<T>());
    }
    // 与另一个链表 x 合并
    // 按照 comp 为真的顺序
    template <class Compare>
    void merge(list& x, Compare comp) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                iterator next = first2;
                _transfer(first1, first2, ++next);
                first2 = next;
            } else {
                ++first1;
            }
        }

        if (first2 != last2) {
            _transfer(last1, first2, last2);
        }
    }

    void reverse() {
        // 如果链表为空或只有一个元素，不需要任何操作
        if (_node->next == _node || _node->next->next == _node) return;

        iterator next_node = begin();
        while (next_node != end()) {
            iterator cur_node = next_node;
            _transfer(begin(), cur_node, ++next_node);
        }
    }

    // STL sort 只接受 RandomAccessIterator，
    // 因此 list 需要实现自己的 sort
    // 本函数采用快排
    void sort() {
    }

private:
    // helper function
    // 创建和销毁节点
    template <class... Args>
    node_ptr _create_node(Args&&... args) {
        node_ptr np = node_allocator::allocate();
        data_allocator::construct(xutl::address_of(np->data),
                                  xutl::forward<Args>(args)...);
        return np;
    }
    void _destroy_node(node_ptr np) {
        data_allocator::destroy(xutl::address_of(np->data));
        node_allocator::deallocate(np);
    }

    // 用 n 个值为 value 的元素初始化
    void _fill_init(size_type n, const_reference value) {
        _node = node_allocator::allocate();
        // 初始化：令 _node 前后都指向自己，并且不设元素值
        _node->prev = _node->next = _node;
        // 一个一个插入
        while (n--) {
            // node_ptr tmp_node = _create_node(value);
            insert(end(), value);
        }
    }

    // 用 [first, last) 区间的元素初始化
    template <class Iterator>
    void _copy_init(Iterator first, Iterator last) {
        _node = node_allocator::allocate();
        _node->prev = _node->next = _node;
        while (first != last) {
            // node_ptr node = _create_node(*first);
            insert(end(), *first);
            ++first;
        }
    }

    // 将 [first, last) 区间的元素移动到 position 之前
    void _transfer(iterator position, iterator first, iterator last) {
        if (position == last) return;  // 如果 last 就是 position，什么都不用做
        // 先把 [first, last) 所属链表的区间前后连接起来
        first.node->prev->next = last.node;
        node_ptr end_node = last.node->prev;
        last.node->prev = first.node->prev;
        // 再断开 [first, last)，连接到 position 之前
        first.node->prev = position.node->prev;
        end_node->next = position.node;
        position.node->prev->next = first.node;
        position.node->prev = end_node;
    }
};
}  // namespace xutl

#endif  // XUTL_LIST_H_