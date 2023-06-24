# XuTL

本项目仅用于个人学习，采用 C++11 标准，主要参考自 [libc++ 3.9.1](https://releases.llvm.org/download.html) 的源码，实现了 STL 的一部分。有空就慢慢完善。

## 文件分布

- [memory.h](XuTL/memory.h)：内存管理相关，包括默认分配器 allocator，allocator_traits，~~智能指针 shared_ptr、unique_ptr、weak_ptr~~（未实现）等。
- [iterator.h](XuTL/iterator.h)：迭代器相关，包括迭代器类别标签类，迭代器基类，iterator_traits，reverse_iterator，迭代器辅助函数 distance、advance、next、prev 等。
- [algorithm.h](XuTL/algorithm.h)：STL 算法相关。
- [type_traits.h](XuTL/type_traits.h)：type_traits 相关。
- [utils.h](XuTL/utils.h)：一些工具函数和类，包括函数 move，forward，swap 等，类 ~~pair~~（未实现）等。
- [construct.h](XuTL/construct.h)：构建和析构对象的函数，包括 construct 和 destroy。
- [exceptdef.h](XuTL/exceptdef.h)：异常相关的宏定义。
- [vector.h](XuTL/vector.h)：容器 vector 相关。

## 内容概览

### Allocator 分配器

该项目主要为了学习，不必迷失于不同分配器中，在写容器时也不必考虑适配各种不同的分配器而陷入复杂的细节。因此本项目只实现一个简单的无状态分配器 allocator，**所有容器默认使用且只能使用该分配器，不允许自定义分配器**。

### Iterator 迭代器

迭代器分为 5 类：**Input Iterator**、**Output Iterator**、**Forward Iterator**、**Bidirectional Iterator** 和 **Random Access Iterator**。

不同容器的数据结构各不相同，所以它们的迭代器可执行操作也会有所不同。比如，vector 的迭代器具有随机访问的性质，而 list 的迭代器只能递增或递减。这样，按照移动特性和可执行操作来分类迭代器就得到了以上 5 类，在需要使用迭代器时，针对不同类别的迭代器使用不同的实现方式，可以最大化程序性能。

### Container 容器

#### 顺序容器

##### vector

本人参考了 libstdc++ 和 libc++ 两个版本的实现。

gcc-11.3.0（libstdc++-v3）中的实现为 vector_base_impl 架构，即 vector 私有继承于 vector_base，vecotr_base 内含 vector_impl 类，而 vector_impl 又公有继承于 allocator 类。

llvm（libc++ 3.9.1）中的实现就是 vector 继承于一个 vector_base，allocator 类放在 compressed_pair 中作为数据成员。

这样的目的在于，vector_impl 类是为了利用**空基类优化**以避免空的分配器类占用额外存储空间。同样，利用 compressed_pair 也可以达成相同的效果。

而 vector_base 是为了 RAII，从而分离内存管理，vector_base 专注于 T 对象内存的分配和释放、vector 本身专注于 T 对象的构造和析构。

至于为什么使用私有继承而不是组合，其实是偏好问题——**私有继承可以说是组合的另一种形式**。标准并未规定必须使用私有继承还是组合。

本项目的 vector 继承于 vector_base，由于本项目的 allocator 是固定的，就是一个简单的无状态分配器，因此不考虑空类优化的问题。另外，本项目不实现 `vector<bool>`。

### Algorithm 算法

目前已手动实现：

1. `copy`
2. `fill_n`

其余暂时直接采用标准库（`using std::xxx`）。

### type_traits

目前已手动实现：

1. `integral_constant`、`true_type`、`false_type`
2. `enable_if`
3. `declval`
4. `is_integral`

其余暂时直接采用标准库（`using std::xxx`）。

## 参考资料

1. LLVM. [*libc++ 3.9.1*](https://releases.llvm.org/download.html).
1. GNU. [*gcc-11.3.0*](https://mirrors.aliyun.com/gnu/gcc/gcc-11.3.0).
2. GNU. [*gcc-2.95*](https://mirrors.aliyun.com/gnu/gcc/gcc-2.95).
3. Alinshans. [*MyTinySTL*](https://github.com/Alinshans/MyTinySTL). Github.
4. 侯捷. *STL 源码剖析*.