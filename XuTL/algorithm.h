#ifndef XUTL_ALGORITHM_H_
#define XUTL_ALGORITHM_H_

#include <algorithm>
#include <cstring>

#include "type_traits.h"

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
// fill_n
// ************************************************************************************

template <typename OutputIterator, typename Size, typename T>
OutputIterator _fill_n(OutputIterator first, Size n, const T& value) {
    while (n--) {
        *first = value;
        ++first;
    }
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
// max, min
// ************************************************************************************

using std::max;
using std::min;

}  // namespace xutl

#endif  // XUTL_ALGORITHM_H_