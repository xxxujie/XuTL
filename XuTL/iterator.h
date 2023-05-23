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

// 常用函数
// 标准规定，作为模板参数的迭代器，以算法所能接受的最低阶迭代器类别命名

// 两个迭代器之间的距离

// 使迭代器前进 n 位

}  // namespace xutl

#endif  // XUTL_ITERATOR_H_