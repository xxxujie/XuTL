#ifndef XUTL_FUNCTIONAL_H_
#define XUTL_FUNCTIONAL_H_

// 该文件包含必要的函数对象（仿函数）

namespace xutl {
// 定义一元函数的参数类型和返回值类型
template <class Arg, class Result>
struct unarg_function {
    using argument_type = Arg;
    using result_type = Result;
};

// 定义二元函数的参数类型和返回值类型
template <class Arg1, class Arg2, class Result>
struct binary_function {
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
    using result_type = Result;
};

// 加减乘除
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x + y;
    }
};
template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x - y;
    }
};
template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x * y;
    }
};
template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x / y;
    }
};

// 取模
template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) {
        return x % y;
    }
};

// 等于
template <class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x == y;
    }
};

// 不等于
template <class T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x != y;
    }
};

// 大于
template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x > y;
    }
};

// 小于
template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x < y;
    }
};

// 大于等于
template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x >= y;
    }
};

// 小于等于
template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x <= y;
    }
};

}  // namespace xutl

#endif  // XUTL_FUNCTIONAL_H_