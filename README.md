# XuTL

该项目仅用于个人学习，采用 C++11 标准，实现了 STL 的一部分功能。

## 计划内容

实现的部分包括：（计划完成）

1. 一个 [`allocator`](XuTL/allocator.h) 类，它是对 `new` 和 `delete` 的封装，其中，用于构造的函数实现于 [construct.h](XuTL/construct.h) 中。
2. 常用的工具函数，包括 `move`, `forward`, `swap`。
3. `pair` 类。它与工具函数一起放在 [utils.h](XuTL/utils.h) 中。

## 参考资料

1. 《STL源码剖析》- 侯捷
2. [MyTinySTL](https://github.com/Alinshans/MyTinySTL) - Alinshans
3. libstdc++ 11.3.0 源码 - gcc