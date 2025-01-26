#pragma once

namespace detail {

template <typename T, bool fast>
struct computeAdd {
    static T call(const T& u, const T& v);
};

template <typename T, bool fast>
struct computeSub {
    static T call(const T& u, const T& v);
};

template <typename T, bool fast>
struct computeMul {
    static T call(const T::type& s, const T& v);
};

template <typename T, bool fast>
struct computeDiv {
    static T call(const T& v, const T::type& s);
};

template <typename T, bool fast>
struct computeEq {
    static bool call(const T& u, const T& v);
};

template <typename T, bool fast>
struct computeNeq {
    static bool call(const T& u, const T& v);
};

template <typename T, bool fast>
struct computeLength {
    static T::type call(const T& v);
};

template <typename T, bool fast>
struct computeDot {
    static T::type call(const T& u, const T& v);
};

};

#include "vec_detail.inl"