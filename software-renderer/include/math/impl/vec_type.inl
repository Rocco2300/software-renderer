#include "vec_type.hpp"

#include <cmath>
#include <cassert>

template <typename T, size_t Size>
vec<T, Size>::vec(const T& value) {
    data[0] = value;
    data[1] = value;
    data[2] = value;
    data[3] = value;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y) {
    static_assert(Size == 2, "Not a vector 2!");

    data[0] = x;
    data[1] = y;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y, const T& z) {
    static_assert(Size == 3, "Not a vector 3!");

    data[0] = x;
    data[1] = y;
    data[2] = z;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y, const T& z, const T& w) {
    static_assert(Size == 4, "Not a vector 4!");

    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const __m128& reg) {
    data = storage<T, Size>(reg);
}

template <typename T, size_t Size>
vec<T, Size>::vec(const vec<T, Size>& other) {
    this->data = other.data;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator=(const vec<T, Size>& other) {
    this->data = other.data;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>::operator T*() {
    return data;
}

template <typename T, size_t Size>
vec<T, Size>::operator __m128() const {
    return data;
}

template <typename T, size_t Size>
T& vec<T, Size>::operator[](size_t index) {
    return data[index];
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator+=(const vec<T, Size>& v) {
    *this = *this + v;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator-=(const vec<T, Size>& v) {
    *this = *this - v;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator*=(float s) {
    *this = *this * s;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator/=(float s) {
    *this = *this / s;
    return *this;
}

// god bless https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
template <typename T, size_t Size>
bool operator==(const vec<T, Size>& u, const vec<T, Size>& v) {
    for (int i = 0; i < Size; i++) {
        if (u[i] != v[i]) {
            return false;
        }
    }

    return true;
}

template <typename T, size_t Size>
bool operator!=(const vec<T, Size>& u, const vec<T, Size>& v) {
    for (int i = 0; i < Size; i++) {
        if (u[i] == v[i]) {
            return false;
        }
    }

    return true;
}

template <typename T, size_t Size>
const T& vec<T, Size>::operator[](size_t index) const {
    return data[index];
}

template <typename T, size_t Size>
vec<T, Size> operator+(const vec<T, Size>& u, const vec<T, Size>& v) {
    vec<T, Size> res(0);

    for (int i = 0; i < Size; i++) {
        res[i] = u[i] + v[i];
    }

    return res;
}

template <typename T, size_t Size>
vec<T, Size> operator-(const vec<T, Size>& u, const vec<T, Size>& v) {
    vec<T, Size> res(0);

    for (int i = 0; i < Size; i++) {
        res[i] = u[i] - v[i];
    }

    return res;
}

template <typename T, size_t Size>
vec<T, Size> operator*(const float& s, const vec<T, Size>& v) {
    vec<T, Size> res(0);

    for (int i = 0; i < Size; i++) {
        res[i] = s * v[i];
    }

    return res;
}

template <typename T, size_t Size>
vec<T, Size> operator*(const vec<T, Size>& v, const float& s) {
    return s * v;
}

template <typename T, size_t Size>
vec<T, Size> operator/(const vec<T, Size>& v, const float& s) {
    vec<T, Size> res(0);

    for (int i = 0; i < Size; i++) {
        res[i] = s / v[i];
    }

    return res;
}

template <typename T, size_t Size>
float length(const vec<T, Size>& v) {
    float res = 0;
    for (int i = 0; i < Size; i++) {
        res += v[i] * v[i];
    }

    return std::sqrt(res);
}

template <typename T, size_t Size>
float dot(const vec<T, Size>& u, const vec<T, Size>& v) {
    float res = 0;
    for (int i = 0; i < Size; i++) {
        res += u[i] * v[i];
    }

    return res;
}

template <typename T, size_t Size>
vec<T, Size> normalize(const vec<T, Size>& v) {
    auto len = length(v);
    return v / len;
}