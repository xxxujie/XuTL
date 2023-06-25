#ifndef XUTL_UTILS_H_
#define XUTL_UTILS_H_

/**
 * 该文件包含常用的工具类和工具函数，包括 move、forward、swap 等函数，pair 等类
 */

#include "type_traits.h"

namespace xutl {

// ************************************************************************************
// forward
// ************************************************************************************

// 接受 lvalue 的版本
template <typename T>
inline T&& forward(typename xutl::remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

// 接受 rvalue 的版本
template <typename T>
inline T&& forward(typename xutl::remove_reference<T>::type&& t) noexcept {
    static_assert(!xutl::is_lvalue_reference<T>::value,
                  "can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(t);
}

// ************************************************************************************
// move
// ************************************************************************************

template <typename T>
inline typename xutl::remove_reference<T>::type&& move(T&& t) noexcept {
    using U = typename xutl::remove_reference<T>::type;
    return static_cast<U&&>(t);
}

// ************************************************************************************
// move
// ************************************************************************************
template <typename T>
void swap(T& lhs, T& rhs) {
    auto tmp = xutl::move(lhs);
    lhs = xutl::move(rhs);
    rhs = xutl::move(tmp);
}

// ************************************************************************************
// address_of
// 取得地址，即转型为内置指针
// ************************************************************************************

template <typename T>
inline T* address_of(T& value) noexcept {
    return &value;
}
}  // namespace xutl

#endif  // XUTL_UTILS_H_