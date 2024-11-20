#pragma once

#include <smmintrin.h>

namespace vec {

template <typename T, size_t Size>
class vec {
private:
    T data[4]{};

public:
    vec() = default;
    vec(const T& x, const T& y);
    vec(const T& x, const T& y, const T& z);
    vec(const T& x, const T& y, const T& z, const T& w);

    vec(const __m128& reg);
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    //vec2 operator+(vec2 v);
    //vec2 operator-(vec2 v);
    //vec2 operator*(float s);
    //vec2 operator/(float s);

    template <typename V, size_t VSize>
    friend bool operator==(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend bool operator!=(const vec<V, VSize>& u, const vec<V, VSize>& v);

    template <typename V, size_t VSize>
    friend vec<V, VSize> operator+(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator-(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const float& s, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const vec<V, VSize>& v, const float& s);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator/(const vec<V, VSize>& v, const float& s);
};

template <typename T, size_t Size>
float length(const vec<T, Size>& v);

template <typename T, size_t Size>
float dot(const vec<T, Size>& u, const vec<T, Size>& v);

template <typename T, size_t Size>
vec<T, Size> normalize(const vec<T, Size>& v);

}; // namespace vec

using vec2 = vec::vec<float, 2>;
using vec3 = vec::vec<float, 3>;
using vec4 = vec::vec<float, 4>;

#include "vec.inl"