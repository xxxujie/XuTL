#ifndef XUTL_ALGORITHM_H_
#define XUTL_ALGORITHM_H_

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

}  // namespace xutl

#endif  // XUTL_ALGORITHM_H_