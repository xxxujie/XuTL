#ifndef XUTL_ALGORITHM_H_
#define XUTL_ALGORITHM_H_

#include <algorithm>
#include <cstddef>
#include <cstring>

#include "iterator.h"
#include "type_traits.h"
#include "utils.h"

namespace xutl {

// ************************************************************************************
// copy
// ************************************************************************************

template <typename InputIterator, typename OutputIterator>
inline OutputIterator _copy(InputIterator first, InputIterator last,
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
_copy(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last,
                           OutputIterator result) {
    return _copy(first, last, result);
}

// ************************************************************************************
// copy_n
// ************************************************************************************

template <typename InputIterator, typename Size, typename OutputIterator>
inline typename enable_if<
    xutl::is_input_iterator<InputIterator>::value &&
        !xutl::is_random_access_iterator<InputIterator>::value,
    OutputIterator>::type
copy_n(InputIterator first, Size n, OutputIterator result) {
    while (n--) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

template <typename RandomAccessIterator, typename Size, typename OutputIterator>
inline typename enable_if<
    xutl::is_random_access_iterator<RandomAccessIterator>::value,
    OutputIterator>::type
copy_n(RandomAccessIterator first, Size n, OutputIterator result) {
    return copy(first, first + n, result);
}

// ************************************************************************************
// copy_backward
// ************************************************************************************

template <typename BidirectionalIterator, typename OutputIterator>
inline OutputIterator _copy_backward(BidirectionalIterator first,
                                     BidirectionalIterator last,
                                     OutputIterator result) {
    while (first != last) {
        --result;
        --last;
        *result = *last;
    }
    return result;
}

template <typename T, typename U>
inline typename enable_if<
    xutl::is_same<typename xutl::remove_const<T>::type, U>::value &&
        xutl::is_trivially_copy_assignable<U>::value,
    U*>::type
_copy_backward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        result -= n;
        std::memmove(result, first, n * sizeof(U));
    }
    return result;
}

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                            BidirectionalIterator1 last,
                                            BidirectionalIterator2 result) {
    return _copy_backward(first, last, result);
}

// ************************************************************************************
// fill_n
// ************************************************************************************

template <typename OutputIterator, typename Size, typename T>
OutputIterator _fill_n(OutputIterator first, Size n, const T& value) {
    while (n--) {
        *first = value;
        ++first;
    }
    return first;
}

template <typename T, typename Size, typename U>
typename enable_if<xutl::is_integral<T>::value && sizeof(T) == 1 &&
                   !xutl::is_same<T, bool>::value &&
                   xutl::is_integral<U>::value && sizeof(U) == 1>::type
_fill_n(T* first, Size n, U value) {
    if (n > 0) {
        memset(first, value, n);
    }
    return first + n;
}

template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    return _fill_n(first, n, value);
}

// ************************************************************************************
// fill
// ************************************************************************************

template <typename ForwardIterator, typename T>
void _fill(ForwardIterator first, ForwardIterator last, const T& value,
           xutl::forward_iterator_tag) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

template <typename RandomAccessIterator, typename T>
void _fill(RandomAccessIterator first, RandomAccessIterator last,
           const T& value, xutl::random_access_iterator_tag) {
    fill_n(first, last - first, value);
}

template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
    _fill(first, last, value,
          typename iterator_traits<ForwardIterator>::iterator_category{});
}

// ************************************************************************************
// move
// ************************************************************************************

template <typename InputIterator, typename OutputIterator>
InputIterator _move(InputIterator first, InputIterator last,
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
_move(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator move(InputIterator first, InputIterator last,
                    OutputIterator result) {
    return xutl::_move(first, last, result);
}

// ************************************************************************************
// move_backward
// ************************************************************************************

template <typename InputIterator, typename OutputIterator>
inline OutputIterator _move_backward(InputIterator first, InputIterator last,
                                     OutputIterator result) {
    while (first != last) {
        --result;
        --last;
        *result = xutl::move(*last);
    }
    return result;
}

template <typename T, typename U>
inline typename enable_if<
    xutl::is_same<typename xutl::remove_const<T>::type, U>::value &&
        xutl::is_trivially_copy_assignable<U>::value,
    U*>::type
_move_backward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n > 0) {
        result -= n;
        std::memmove(result, first, n * sizeof(U));
    }
    return result;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator move_backward(InputIterator first, InputIterator last,
                                    OutputIterator result) {
    return _move_backward(first, last, result);
}

// ************************************************************************************
// max, min
// ************************************************************************************

using std::max;
using std::min;

}  // namespace xutl

#endif  // XUTL_ALGORITHM_H_