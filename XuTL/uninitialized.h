#ifndef XUTL_UNINITIALIZED_H_
#define XUTL_UNINITIALIZED_H_

// 该头文件包含多个辅助函数，用于处理未初始化空间，在其中构造对象

#include "algorithm.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "utils.h"

namespace xutl {

// ************************************************************************************
// uinitialized_copy
// 把 [first, last) 中的内容复制到以 result 为起点的空间，返回复制结束的位置
// ************************************************************************************

template <typename InputIterator, typename ForwardIterator>
ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last,
                                    ForwardIterator result, xutl::true_type) {
    return xutl::copy(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last,
                                    ForwardIterator result) {
    ForwardIterator current = result;
    try {
        while (first != last) {
            // &*current 把迭代器变成普通指针
            xutl::construct(&*current, *first);
            ++first;
            ++current;
        }
    } catch (...) {
        // 出现异常的话，需要析构所有已经构造的对象
        xutl::destroy(result, current);
    }
    return current;
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    return _uninitialized_copy(
        first, last,
        xutl::is_trivially_copy_assignable<
            typename iterator_traits<InputIterator>::value_type>{});
}

// ************************************************************************************
// uinitialized_copy_n
// 把 [first, first + n) 中的内容复制到以 result
// 为起点的空间，返回复制结束的位置
// ************************************************************************************

template <typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator _uninitialized_copy_n(InputIterator first, Size n,
                                      ForwardIterator result, xutl::true_type) {
    return xutl::copy_n(first, n, result);
}

template <typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator _uninitialized_copy_n(InputIterator first, Size n,
                                      ForwardIterator result,
                                      xutl::false_type) {
    ForwardIterator current = result;
    try {
        while (n--) {
            xutl::construct(&*current, *first);
            ++first;
            ++current;
        }
    } catch (...) {
        xutl::destroy(result, current);
    }
    return current;
}

template <typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator uninitialized_copy_n(InputIterator first, Size n,
                                     ForwardIterator result) {
    return _uninitialized_copy_n(
        first, n, result,
        xutl::is_trivially_copy_assignable<
            typename iterator_traits<InputIterator>::value_type>{});
}

// ************************************************************************************
// uinitialized_fill
// 用 [first, last) 中构造值为 value 的对象
// ************************************************************************************

template <typename ForwardIterator, typename T>
void _uninitialized_fill(ForwardIterator first, ForwardIterator last,
                         const T& value, xutl::true_type) {
    xutl::fill(first, last, value);
}

template <typename ForwardIterator, typename T>
void _uninitialized_fill(ForwardIterator first, ForwardIterator last,
                         const T& value, xutl::false_type) {
    ForwardIterator current = first;
    try {
        while (current != last) {
            xutl::construct(&*current, value);
            ++current;
        }
    } catch (...) {
        xutl::destroy(first, current);
    }
}

template <typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                        const T& value) {
    _uninitialized_fill(
        first, last, value,
        xutl::is_trivially_copy_assignable<
            typename xutl::iterator_traits<ForwardIterator>::value_type>{});
}

// ************************************************************************************
// uinitialized_fill_n
// 用 [first, first + n) 中构造值为 value 的对象，返回构造结束的位置
// ************************************************************************************

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n,
                                      const T& value, xutl::true_type) {
    return fill_n(first, n, value);
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n,
                                      const T& value, xutl::false_type) {
    ForwardIterator current = first;
    try {
        while (n--) {
            xutl::construct(&*current, value);
            ++current;
        }
    } catch (...) {
        xutl::destroy(first, current);
    }
    return current;
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
                                     const T& value) {
    return _uninitialized_fill_n(
        first, n, value,
        xutl::is_trivially_copy_assignable<
            typename xutl::iterator_traits<ForwardIterator>::value_type>{});
}

// ************************************************************************************
// uinitialized_move
// 用 [first, first + n) 中构造值为 value 的对象，返回构造结束的位置
// ************************************************************************************

template <typename InputIterator, typename ForwardIterator>
ForwardIterator _uninitialized_move(InputIterator first, InputIterator last,
                                    ForwardIterator result, xutl::true_type) {
    return xutl::move(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator _uninitialized_move(InputIterator first, InputIterator last,
                                    ForwardIterator result, xutl::false_type) {
    ForwardIterator current = result;
    try {
        while (first != last) {
            xutl::construct(&*current, xutl::move(*first));
            ++first;
            ++current;
        }
    } catch (...) {
        xutl::destroy(result, current);
    }
    return current;
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    return _uninitialized_move(
        first, last, result,
        xutl::is_trivially_move_assignable<
            typename xutl::iterator_traits<InputIterator>::value_type>{});
}

}  // namespace xutl

#endif  // XUTL_UNINITIALIZED_H_