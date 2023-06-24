#ifndef XUTL_ITERATOR_H_
#define XUTL_ITERATOR_H_

/**
 * 该文件包含迭代器相关的东西, 包括五种迭代器类别标签，一个迭代器类模板, 以及
 * iterator_traits
 */

#include <cstddef>

#include "type_traits.h"

namespace xutl {

// ************************************************************************************
// 五种迭代器类别标签, 以类的形式表示
// ************************************************************************************

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// ************************************************************************************
// iterator 基类
// ************************************************************************************

template <typename Category, typename T, typename Distance = std::ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
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

// ************************************************************************************
// iterator traits
// ************************************************************************************

// 类型 T 是否具有内嵌类型 iterator_category
template <typename T>
class has_iterator_category {
private:
    struct two {
        char a;
        char b;
    };
    // 利用 SFINAE，只有类型具备内嵌类型 iterator_category
    // 时，才会实例化第二个函数模板
    template <typename U>
    static two test(...);
    template <typename U>
    static char test(typename U::iterator_category* = nullptr);

public:
    // 具有内嵌类型 iterator_category 时，返回类型为 char，size 与 char
    // 相等，value 为 true； 否则，返回类型为 two，size 与 char 不相等，value 为
    // false
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <typename Iterator, bool>
struct _iterator_traits_impl {};

template <typename Iterator>
struct _iterator_traits_impl<Iterator, true> {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

template <typename Iterator, bool>
struct _iterator_traits {};

template <typename Iterator>
struct _iterator_traits<Iterator, true>
    : public _iterator_traits_impl<
          Iterator,
          xutl::is_convertible<typename Iterator::iterator_category,
                               input_iterator_tag>::value ||
              xutl::is_convertible<typename Iterator::iterator_category,
                                   output_iterator_tag>::value> {};

template <typename Iterator>
struct iterator_traits
    : public _iterator_traits<Iterator,
                              has_iterator_category<Iterator>::value> {};

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

template <typename IteratorFrom, typename CategoryTo,
          bool = has_iterator_category<iterator_traits<IteratorFrom>>::value>
struct has_iterator_category_convertible_to
    : public xutl::integral_constant<
          bool, xutl::is_convertible<
                    typename iterator_traits<IteratorFrom>::iterator_category,
                    CategoryTo>::value> {};

template <typename IteratorFrom, typename CategoryTo>
struct has_iterator_category_convertible_to<IteratorFrom, CategoryTo, false>
    : public false_type {};

// 判断迭代器类别

template <typename Iterator>
struct is_input_iterator
    : public has_iterator_category_convertible_to<Iterator,
                                                  input_iterator_tag> {};

template <typename Iterator>
struct is_forward_iterator
    : public has_iterator_category_convertible_to<Iterator,
                                                  forward_iterator_tag> {};

template <typename Iterator>
struct is_bidirectional_iterator
    : public has_iterator_category_convertible_to<Iterator,
                                                  bidirectional_iterator_tag> {
};

template <typename Iterator>
struct is_random_access_iterator
    : public has_iterator_category_convertible_to<Iterator,
                                                  random_access_iterator_tag> {
};

template <typename Iterator>
struct is_exactly_input_iterator
    : public integral_constant<bool,
                               has_iterator_category_convertible_to<
                                   Iterator, input_iterator_tag>::value &&
                                   !has_iterator_category_convertible_to<
                                       Iterator, forward_iterator_tag>::value> {
};

// ************************************************************************************
// 迭代器的 helper function
// 标准规定，作为模板参数的迭代器，以算法所能接受的最低阶迭代器类别命名
// ************************************************************************************

// distance：两个迭代器之间的距离

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type _distance(
    InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type diff = 0;
    while (first != last) {
        ++first;
        ++diff;
    }
    return diff;
}

template <typename RandomAccessIterator>
typename iterator_traits<RandomAccessIterator>::difference_type _distance(
    RandomAccessIterator first, RandomAccessIterator last,
    random_access_iterator_tag) {
    return last - first;
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(
    InputIterator first, InputIterator last) {
    return _distance(
        first, last,
        typename iterator_traits<InputIterator>::iterator_category());
}

// advance：使迭代器前进 n 位

template <typename InputIterator, typename Distance>
void _advance(InputIterator& it, Distance n, input_iterator_tag) {
    while (n--) {
        ++it;
    }
}

template <typename BidirectionalIterator, typename Distance>
void _advance(BidirectionalIterator it, Distance n,
              bidirectional_iterator_tag) {
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

template <typename InputIterator, typename Distance>
void advance(InputIterator& it, Distance n) {
    _advance(it, n, iterator_category(it));
}

// next：当前迭代器的后 n 位，n 默认为 1

template <typename InputIterator>
inline InputIterator next(
    InputIterator it,
    typename iterator_traits<InputIterator>::difference_type n = 1) {
    advance(it, n);
    return it;
}

// prev：当前迭代器的前 n 位，n 默认为 1

template <typename BidirectionalIterator>
inline BidirectionalIterator prev(
    BidirectionalIterator it,
    typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
    advance(it, -n);
    return it;
}

}  // namespace xutl

#endif  // XUTL_ITERATOR_H_