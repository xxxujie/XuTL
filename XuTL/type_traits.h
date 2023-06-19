#ifndef XUTL_TYPE_TRAITS_H_
#define XUTL_TYPE_TRAITS_H_

namespace xutl {
// enable_if
template <bool, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

}  // namespace xutl

#endif  // XUTL_TYPE_TRAITS_H_