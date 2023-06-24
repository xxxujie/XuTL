#ifndef XUTL_ALGORITHM_H_
#define XUTL_ALGORITHM_H_

#include <algorithm>
#include <cstring>

#include "iterator.h"
#include "type_traits.h"
#include "utils.h"

namespace xutl {

// ************************************************************************************
// copy_forward
// ************************************************************************************

template <typename InputIterator, typename OutputIterator>
inline OutputIterator _copy_forward(InputIterator first, InputIterator last,
                                    OutputIterator result) {
    while (first != last) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

template <typename T, typename U>
inline typename enable_if<
    xutl::is_same<typename xutl::remove_const<T>, U>::value &&
        xutl::is_trivially_copy_assignable<U>::value,
    U*>::type
_copy_forward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy_forward(InputIterator first, InputIterator last,
                                   OutputIterator result) {
    return _copy_forward(first, last, result);
}

// ************************************************************************************
// copy_forward_n
// ************************************************************************************

template <typename InputIterator, typename Size, typename OutputIterator>
OutputIterator copy_forward_n(InputIterator first, Size n,
                              OutputIterator result) {
    while (n--) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

// ************************************************************************************
// fill_forward_n
// ************************************************************************************

template <typename OutputIterator, typename Size, typename T>
OutputIterator _fill_forward_n(OutputIterator first, Size n, const T& value) {
    while (n--) {
        *first = value;
        ++first;
    }
}

template <typename T, typename Size, typename U>
typename enable_if<xutl::is_integral<T>::value && sizeof(T) == 1 &&
                   !xutl::is_same<T, bool>::value &&
                   xutl::is_integral<U>::value && sizeof(U) == 1>::type
_fill_forward_n(T* first, Size n, U value) {
    if (n > 0) {
        memset(first, value, n);
    }
    return first + n;
}

template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_forward_n(OutputIterator first, Size n, const T& value) {
    return _fill_forward_n(first, n, value);
}

// ************************************************************************************
// fill_forward
// ************************************************************************************

template <typename ForwardIterator, typename T>
void _fill_forward(ForwardIterator first, ForwardIterator last, const T& value,
                   xutl::forward_iterator_tag) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

template <typename RandomAccessIterator, typename T>
void _fill_forward(RandomAccessIterator first, RandomAccessIterator last,
                   const T& value, xutl::random_access_iterator_tag) {
    fill_forward_n(first, last - first, value);
}

template <typename ForwardIterator, typename T>
void fill_forward(ForwardIterator first, ForwardIterator last, const T& value) {
    _fill_forward(
        first, last, value,
        typename iterator_traits<ForwardIterator>::iterator_category{});
}

// ************************************************************************************
// move_forward
// ************************************************************************************

template <typename InputIterator, typename OutputIterator>
InputIterator _move_forward(InputIterator first, InputIterator last,
                            OutputIterator result) {
    while (first != last) {
        *result = xutl::move(*first);
        ++result;
        ++first;
    }
    return result;
}

template <typename T, typename U>
inline typename enable_if<
    xutl::is_same<typename xutl::remove_const<T>, U>::value &&
        xutl::is_trivially_move_assignable<U>::value,
    U*>::type
_move_forward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator move_forward(InputIterator first, InputIterator last,
                            OutputIterator result) {
    xutl::_move_forward(first, last, result);
}

// ************************************************************************************
// max, min
// ************************************************************************************

using std::max;
using std::min;

}  // namespace xutl

#endif  // XUTL_ALGORITHM_H_