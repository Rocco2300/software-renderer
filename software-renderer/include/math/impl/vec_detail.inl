#include "vec_detail.hpp"

#include <cmath>

namespace detail {

template <typename T>
struct computeAdd<T, true> {
    static T call(const T& u, const T& v) {
        return _mm_add_ps(u, v);
    }
};

template <typename T>
struct computeAdd<T, false> {
    static T call(const T& u, const T& v) {
        T res;
        for (int i = 0; i < T::size; i++) {
            res[i] = u[i] + v[i];
        }
        return res;
    }
};

template <typename T>
struct computeSub<T, true> {
    static T call(const T& u, const T& v) {
        return _mm_sub_ps(u, v);
    }
};

template <typename T>
struct computeSub<T, false> {
    static T call(const T& u, const T& v) {
        T res;
        for (int i = 0; i < T::size; i++) {
            res[i] = u[i] - v[i];
        }
        return res;
    }
};

template <typename T>
struct computeMul<T, true> {
    static T call(const T::type& s, const T& v) {
        auto scalar = _mm_load1_ps(&s);
        return _mm_mul_ps(v, scalar);
    }
};

template <typename T>
struct computeMul<T, false> {
    static T call(const T::type& s, const T& v) {
        T res;
        for (int i = 0; i < T::size; i++) {
            res[i] = s * v[i];
        }
        return res;
    }
};

template <typename T>
struct computeDiv<T, true> {
    static T call(const T& v, const T::type& s) {
        auto scalar = _mm_load1_ps(&s);
        return _mm_div_ps(v, scalar);
    }
};

template <typename T>
struct computeDiv<T, false> {
    static T call(const T& v, const T::type& s) {
        T res;
        for (int i = 0; i < T::size; i++) {
            res[i] = v[i] / s;
        }
        return res;
    }
};

template <typename T>
struct computeEq<T, true> {
    static bool call(const T& u, const T& v) {
        auto eps = _mm_set1_ps(0.001f);
        auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
        return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) == 0xF;
    }
};

template <typename T>
struct computeEq<T, false> {
    static bool call(const T& u, const T& v) {
        for (int i = 0; i < T::size; i++) {
            if (u[i] != v[i]) {
                return false;
            }
        }
        return true;
    }
};

template <typename T>
struct computeNeq<T, true> {
    static bool call(const T& u, const T& v) {
        auto eps = _mm_set1_ps(0.001f);
        auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
        return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) != 0xF;
    }
};

template <typename T>
struct computeNeq<T, false> {
    static bool call(const T& u, const T& v) {
        for (int i = 0; i < T::size; i++) {
            if (u[i] == v[i]) {
                return false;
            }
        }
        return true;
    }
};

template <typename T>
struct computeLength<T, true> {
    static T::type call(const T& v) {
        auto sq  = _mm_mul_ps(v, v);
        auto sum = _mm_hadd_ps(sq, sq);
        sum      = _mm_hadd_ps(sum, sum);

        return std::sqrt(_mm_cvtss_f32(sum));
    }
};

template <typename T>
struct computeLength<T, false> {
    static T::type call(const T& v) {
        typename T::type sum{};
        for (int i = 0; i < T::size; i++) {
            sum += v[i] * v[i];
        }
        return std::sqrt(sum);
    }
};

template <typename T>
struct computeDot<T, true> {
    static T::type call(const T& u, const T& v) {
        auto result = _mm_dp_ps(u, v, 0xFF);
        return _mm_cvtss_f32(result);
    }
};


template <typename T>
struct computeDot<T, false> {
    static T::type call(const T& u, const T& v) {
        typename T::type sum{};
        for (int i = 0; i < T::size; i++) {
            sum += u[i] * v[i];
        }
        return sum;
    }
};

};
