#ifndef XUTL_DEBUGDEF_H_
#define XUTL_DEBUGDEF_H_

/**
 * 该文件包含一些用于 debug 的宏
 */

#include <cassert>

namespace xutl {
#define XUTL_DEBUG_ASSERT(expr) assert(expr);
}  // namespace xutl

#endif  // XUTL_DEBUGDEF_H_