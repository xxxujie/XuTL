#ifndef XUTL_ITERATOR_H_
#define XUTL_ITERATOR_H_

/**
 * 该文件包含迭代器相关的东西, 包括五种迭代器类别标签，一个迭代器类模板, 以及 iterator_traits
 */

#include <cstddef>

namespace xutl {

// 五种迭代器类别标签, 以类的形式表示

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 通用 iterator template, 所有 iterator 定义都要继承它

template <typename Category, typename T, typename Distance = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator {
    // 五种迭代器类别标签之一
    using iterator_category = Category;
    // 迭代器所指类型
    using value_type = T;
    // 迭代器之间的距离以这种类型表示
    using difference_type = Distance;
    // 迭代器所指类型的指针
    using pointer = Pointer;
    // 迭代器所指类型的引用
    using reference = Reference;
};

// iterator traits

template <typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

// 针对 pointer 的偏特化
template <typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// 针对 const pointer 的偏特化
template <typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

// 仅供内部库使用的语法糖
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
    using category = typename iterator_traits<Iterator>::iterator_category;
    return category{};
}

template <typename Iterator>
using _iterator_category_t = typename iterator_traits<Iterator>::iterator_category;

// 迭代器相关算法
// 标准规定，作为模板参数的迭代器，以算法所能接受的最低阶迭代器类别命名

// 两个迭代器之间的距离
template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(InputIterator first,
                                                                  InputIterator last) {
    return _distance(first, last, iterator_category(first));
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type _distance(InputIterator first,
                                                                   InputIterator last,
                                                                   input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type diff = 0;
    while (first != last) {
        ++first;
        ++diff;
    }
    return diff;
}

template <typename RandomAccessIterator>
typename iterator_traits<RandomAccessIterator>::difference_type _distance(
    RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

// 使迭代器前进 n 位
template <typename InputIterator, typename Distance>
void advance(InputIterator& it, Distance n) {
    _advance(it, n, iterator_category(it));
}

template <typename InputIterator, typename Distance>
void _advance(InputIterator& it, Distance n, input_iterator_tag) {
    while (n--) {
        ++it;
    }
}

template <typename BidirectionalIterator, typename Distance>
void _advance(BidirectionalIterator it, Distance n, bidirectional_iterator_tag) {
    if (n > 0) {
        while (n--) {
            ++it;
        }
    } else {
        while (n++) {
            --it;
        }
    }
}

template <typename RandomAccessIterator, typename Distance>
void _advance(RandomAccessIterator it, Distance n, random_access_iterator_tag) {
    it += n;
}

template <typename InputIterator>
inline InputIterator next(InputIterator it,
                          typename iterator_traits<InputIterator>::difference_type n = 1) {
    advance(it, n);
    return it;
}

template <typename BidirectionalIterator>
inline BidirectionalIterator prev(
    BidirectionalIterator it,
    typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
    advance(it, -n);
    return it;
}

}  // namespace xutl

#endif  // XUTL_ITERATOR_H_