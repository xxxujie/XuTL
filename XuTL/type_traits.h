#ifndef XUTL_TYPE_TRAITS_H_
#define XUTL_TYPE_TRAITS_H_

#include <type_traits>

namespace xutl {

// helper class:

template <typename T, T v>
struct integral_constant {
    static constexpr const T value = v;
    using value_type = T;
    using type = integral_constant;
    // 转型函数
    inline constexpr operator value_type() const noexcept {
        return value;
    }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

// enable_if

template <bool, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

// is_convertible
using std::is_convertible;

// is_same
using std::is_same;

// is_trivially_move_constructibe
using std::is_trivially_move_constructible;

// is_lvalue_reference;
using std::is_lvalue_reference;

// declval
// 只有声明，没有定义，不可 ODR-use，
// 只能用于 unevaluated context 的函数，
// 返回 T 类型的右值引用

template <typename T>
T&& _declval(int);

template <typename T>
T _declval(long);

template <typename T>
decltype(xutl::_declval<T>(0)) declval() noexcept;

// remove_cv
using std::remove_const;
using std::remove_cv;

// remove_reference
using std::remove_reference;

}  // namespace xutl

#endif  // XUTL_TYPE_TRAITS_H_