#ifndef XUTL_ALGOBASE_H_
#define XUTL_ALGOBASE_H_

#include "iterator.h"
#include "utils.h"

/**
 * 该文件包含 xutl 的基本算法
 */

namespace xutl {

// copy
// 把 [first, last) 中的元素拷贝到以 result 为起点的空间, 返回拷贝结束的位置

template <typename InputIter, typename OutputIter>
OutputIter copy(InputIter first, InputIter last, OutputIter result) {
}

}  // namespace xutl

#endif  // XUTL_ALGOBASE_H_