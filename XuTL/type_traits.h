#ifndef XUTL_TYPE_TRAITS_H_
#define XUTL_TYPE_TRAITS_H_

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

}  // namespace xutl

#endif  // XUTL_TYPE_TRAITS_H_