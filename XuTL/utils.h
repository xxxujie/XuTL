#ifndef XUTL_UTILS_H_
#define XUTL_UTILS_H_

/**
 * 该文件包含常用的工具类和工具函数，包括 move、forward、swap 等函数，pair 等类
 */

#include <type_traits>

namespace xutl {

// forward

// 接受 lvalue 的版本
template <typename T>
inline T&& forward(typename std::remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

// 接受 rvalue 的版本
template <typename T>
inline T&& forward(typename std::remove_reference<T>::type&& t) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(t);
}

}  // namespace xutl

#endif  // XUTL_UTILS_H_