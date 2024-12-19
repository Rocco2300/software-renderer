#pragma once

#include <smmintrin.h>

template <typename T, size_t Size>
struct storage {
    T data[Size]{};

    //template <typename... Args>
    //storage(Args&&... args);
    storage() = default;
    storage(const __m128& reg);
    storage(const storage<T, Size>& other);

    storage<T, Size>& operator=(const __m128& reg);
    storage<T, Size>& operator=(const storage<T, Size>& other);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;
};

#include "storage.inl"
