#ifndef XUTL_CONSTRUCT_H_
#define XUTL_CONSTRUCT_H_

/**
 * 该文件包含两个工具函数，给 allcator 使用，包括 construct 和 destroy
 * construct: 用于构造对象
 * destroy: 用于析构对象
 */

#include <new>

#include "utils.h"

namespace xutl {

template <typename T>
inline void construct(T* ptr) {
    new (ptr) T();
}

template <typename T1, typename T2>
inline void construct(T1* ptr, const T2& value) {
    new (ptr) T1(value);
}

template <typename T, typename... Args>
inline void construct(T* ptr, Args&&... value_packet) {
    new (ptr) T();
}

}  // namespace xutl

#endif  // XUTL_CONSTRUCT_H_