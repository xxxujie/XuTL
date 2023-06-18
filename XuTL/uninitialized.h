#ifndef XUTL_UNINITIALIZED_H_
#define XUTL_UNINITIALIZED_H_

/**
 * 该文件包含一些工具函数，用于在未初始化空间构造元素
 */

#include <type_traits>
namespace xutl {
// uninitialized_copy
// 把 [first, last) 上的内容拷贝到以 result 为起点的空间, 返回拷贝结束的位置

template <typename InputIter, typename ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result,
                                  std::true_type) {
}

template <typename ForwardIterator, typename Size, typename T>
inline void uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
}

}  // namespace xutl

#endif  // XUTL_UNINITIALIZED_H_