#ifndef XUTL_DEBUGDEF_H_
#define XUTL_DEBUGDEF_H_

/**
 * 该文件包含异常相关的宏定义
 */

#include <cassert>
#include <stdexcept>

namespace xutl {

#define XUTL_DEBUG_ASSERT(expr) assert(expr)

#define THROW_LENGTH_ERROR(what) throw std::length_error(what)

}  // namespace xutl

#endif  // XUTL_DEBUGDEF_H_