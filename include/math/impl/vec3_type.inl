#include "vec3_type.hpp"

#include <cmath>
#include <cassert>

template <typename T>
vec<T, 3>::vec(const T& value) {
    data[0] = value;
    data[1] = value;
    data[2] = value;
}

template <typename T>
vec<T, 3>::vec(const T& x, const T& y, const T& z) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

template <typename T>
vec<T, 3>::vec(const __m128& reg) {
    data = storage<T, 3>(reg);
}

template <typename T>
vec<T, 3>::vec(const vec<T, 3>& other) {
    this->data = other.data;
}

template <typename T>
vec<T, 3>& vec<T, 3>::operator=(const vec<T, 3>& other) {
    this->data = other.data;
    return *this;
}

template <typename T>
vec<T, 3>::operator T*() {
    return data;
}

template <typename T>
vec<T, 3>::operator __m128() const {
    return data;
}

template <typename T>
T& vec<T, 3>::operator[](size_t index) {
    return data[index];
}

template <typename T>
vec<T, 3>& vec<T, 3>::operator+=(const vec<T, 3>& v) {
    *this = *this + v;
    return *this;
}

template <typename T>
vec<T, 3>& vec<T, 3>::operator-=(const vec<T, 3>& v) {
    *this = *this - v;
    return *this;
}

template <typename T>
vec<T, 3>& vec<T, 3>::operator*=(float s) {
    *this = *this * s;
    return *this;
}

template <typename T>
vec<T, 3>& vec<T, 3>::operator/=(float s) {
    *this = *this / s;
    return *this;
}

// god bless https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
template <typename T>
bool operator==(const vec<T, 3>& u, const vec<T, 3>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) == 0xF;
}

template <typename T>
bool operator!=(const vec<T, 3>& u, const vec<T, 3>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) != 0xF;
}

template <typename T>
const T& vec<T, 3>::operator[](size_t index) const {
    return data[index];
}

template <typename T>
vec<T, 3> operator+(const vec<T, 3>& u, const vec<T, 3>& v) {
    return _mm_add_ps(u, v);
}

template <typename T>
vec<T, 3> operator-(const vec<T, 3>& u, const vec<T, 3>& v) {
    return _mm_sub_ps(u, v);
}

template <typename T>
vec<T, 3> operator*(const float& s, const vec<T, 3>& v) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_mul_ps(v, scalar);
}

template <typename T>
vec<T, 3> operator*(const vec<T, 3>& v, const float& s) {
    return s * v;
}

template <typename T>
vec<T, 3> operator/(const vec<T, 3>& v, const float& s) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_div_ps(v, scalar);
}
