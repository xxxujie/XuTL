# XuTL

该项目仅用于个人学习，采用 C++11 标准，实现了 STL 的一部分。有空就慢慢完善。

## 实现内容

### Allocator 分配器

1. allocator 类，它是对 `new` 和 `delete` 的封装，位于 [allocator.h](XuTL/allocator.h)。
2. 用于构造和析构对象的函数，位于 [construct.h](XuTL/construct.h)。
3. allocator_traits。

### Iterator 迭代器

1. iterator 基类。
2. iterator_traits。
3. distance 函数。
4. advance 函数。

均位于 [iterator.h](XuTL/iterator.h)。

### Container 容器

1. vector 类，参考自 gcc-11.3.0，采用 vector_base_impl 架构（具体解释见代码注释），简化了 allocator 逻辑，直接限定所有 allocator 的值类型与 T 相同。位于 [vector.h](XuTL/vector.h)。

### Algorithm 算法

1. 一些基本算法，位于 [algobase.h](XuTL/algobase.h)。


### 工具类/函数

1. 常用的工具函数，包括 `move`、`forward`、`swap`，位于 [utils.h](XuTL/utils.h)。
2. 用于在未初始化空间构造元素的函数，位于 [uninitialized.h](XuTL/uninitialized.h)。


## 参考资料

1. GNU. [*gcc-11.3.0*](https://mirrors.aliyun.com/gnu/gcc/gcc-11.3.0).
2. GNU. [*gcc-2.95*](https://mirrors.aliyun.com/gnu/gcc/gcc-2.95).
3. 侯捷. *STL 源码剖析*.
4. Alinshans. [*MyTinySTL*](https://github.com/Alinshans/MyTinySTL). Github.