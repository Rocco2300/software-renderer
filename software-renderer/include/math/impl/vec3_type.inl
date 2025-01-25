#include "vec3_type.hpp"

#include <cmath>
#include <cassert>

inline vec<float, 3>::vec(const float& value) {
    data[0] = value;
    data[1] = value;
    data[2] = value;
}

inline vec<float, 3>::vec(const float& x, const float& y, const float& z) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

inline vec<float, 3>::vec(const __m128& reg) {
    data = storage<float, 3>(reg);
}

inline vec<float, 3>::vec(const vec<float, 3>& other) {
    this->data = other.data;
}

template <typename T>
inline vec<float, 3>::vec(const vec<T, 4>& other) {
    this->data[0] = other.data[0];
    this->data[1] = other.data[1];
    this->data[2] = other.data[2];
}

inline vec<float, 3>& vec<float, 3>::operator=(const vec<float, 3>& other) {
    this->data = other.data;
    return *this;
}

inline vec<float, 3>::operator float*() {
    return data;
}

inline vec<float, 3>::operator __m128() const {
    return data;
}

inline float& vec<float, 3>::operator[](size_t index) {
    return data[index];
}

inline vec<float, 3>& vec<float, 3>::operator+=(const vec<float, 3>& v) {
    *this = *this + v;
    return *this;
}

inline vec<float, 3>& vec<float, 3>::operator-=(const vec<float, 3>& v) {
    *this = *this - v;
    return *this;
}

inline vec<float, 3>& vec<float, 3>::operator*=(float s) {
    *this = *this * s;
    return *this;
}

inline vec<float, 3>& vec<float, 3>::operator/=(float s) {
    *this = *this / s;
    return *this;
}

// god bless https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
inline bool operator==(const vec<float, 3>& u, const vec<float, 3>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) == 0xF;
}

inline bool operator!=(const vec<float, 3>& u, const vec<float, 3>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) != 0xF;
}

inline const float& vec<float, 3>::operator[](size_t index) const {
    return data[index];
}

inline vec<float, 3> operator+(const vec<float, 3>& u, const vec<float, 3>& v) {
    return _mm_add_ps(u, v);
}

inline vec<float, 3> operator-(const vec<float, 3>& u, const vec<float, 3>& v) {
    return _mm_sub_ps(u, v);
}

inline vec<float, 3> operator*(const float& s, const vec<float, 3>& v) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_mul_ps(v, scalar);
}

inline vec<float, 3> operator*(const vec<float, 3>& v, const float& s) {
    return s * v;
}

inline vec<float, 3> operator/(const vec<float, 3>& v, const float& s) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_div_ps(v, scalar);
}