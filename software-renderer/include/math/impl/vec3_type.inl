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
vec<T, 3>::vec(const vec<T, 4>& other) {
    this->data[0] = other.data[0];
    this->data[1] = other.data[1];
    this->data[2] = other.data[2];
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
    return detail::computeEq<vec<T, 3>, std::is_same<T, float>::value>::call(u, v);
}

template <typename T>
bool operator!=(const vec<T, 3>& u, const vec<T, 3>& v) {
    return detail::computeNeq<vec<T, 3>, std::is_same<T, float>::value>::call(u, v);
}

template <typename T>
const T& vec<T, 3>::operator[](size_t index) const {
    return data[index];
}

template <typename T>
vec<T, 3> operator+(const vec<T, 3>& u, const vec<T, 3>& v) {
    return detail::computeAdd<vec<T, 3>, std::is_same<T, float>::value>::call(u, v);
}

template <typename T>
vec<T, 3> operator-(const vec<T, 3>& u, const vec<T, 3>& v) {
    return detail::computeSub<vec<T, 3>, std::is_same<T, float>::value>::call(u, v);
}

template <typename T>
vec<T, 3> operator*(const float& s, const vec<T, 3>& v) {
    return detail::computeMul<vec<T, 3>, std::is_same<T, float>::value>::call(s, v);
}

template <typename T>
vec<T, 3> operator*(const vec<T, 3>& v, const float& s) {
    return s * v;
}

template <typename T>
vec<T, 3> operator/(const vec<T, 3>& v, const float& s) {
    return detail::computeDiv<vec<T, 3>, std::is_same<T, float>::value>::call(v, s);
}
