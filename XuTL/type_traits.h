#ifndef XUTL_TYPE_TRAITS_H_
#define XUTL_TYPE_TRAITS_H_

#include <type_traits>

namespace xutl {

// helper class:

// 标准库的谓词必须使用标准库的 true_type 和
// false_type，未手动实现全部谓词前，不手动实现以下代码

// template <typename T, T v>
// struct integral_constant {
//     static constexpr const T value = v;
//     using value_type = T;
//     using type = integral_constant;
//     // 转型函数
//     constexpr operator value_type() const noexcept {
//         return value;
//     }
// };

// using true_type = integral_constant<bool, true>;
// using false_type = integral_constant<bool, false>;

using std::integral_constant;

using std::false_type;
using std::true_type;

// enable_if

template <bool, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

// is_same
using std::is_same;

// is_convertible
using std::is_convertible;

// is_trivially_move_constructibe
using std::is_trivially_move_constructible;

// is_lvalue_reference;
using std::is_lvalue_reference;

// is_trivially_assignable
using std::is_trivially_assignable;

// is_trivially_copy_assignable
using std::is_trivially_copy_assignable;

// is_trivially_move_assignable
using std::is_trivially_move_assignable;

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

// is_integral

template <class T>
struct _is_integral : public false_type {};
template <>
struct _is_integral<bool> : public true_type {};
template <>
struct _is_integral<char> : public true_type {};
template <>
struct _is_integral<signed char> : public true_type {};
template <>
struct _is_integral<unsigned char> : public true_type {};
template <>
struct _is_integral<wchar_t> : public true_type {};
template <>
struct _is_integral<char16_t> : public true_type {};
template <>
struct _is_integral<char32_t> : public true_type {};
template <>
struct _is_integral<short> : public true_type {};
template <>
struct _is_integral<unsigned short> : public true_type {};
template <>
struct _is_integral<int> : public true_type {};
template <>
struct _is_integral<unsigned int> : public true_type {};
template <>
struct _is_integral<long> : public true_type {};
template <>
struct _is_integral<unsigned long> : public true_type {};
template <>
struct _is_integral<long long> : public true_type {};
template <>
struct _is_integral<unsigned long long> : public true_type {};
template <>
struct _is_integral<__int128_t> : public true_type {};
template <>
struct _is_integral<__uint128_t> : public true_type {};

template <class T>
struct is_integral : public _is_integral<typename remove_cv<T>::type> {};

}  // namespace xutl

#endif  // XUTL_TYPE_TRAITS_H_