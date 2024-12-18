#pragma once

#include <smmintrin.h>

template <typename T, size_t Size>
struct storage {
    T data[4]{};

    template <typename... Args>
    storage(Args&&... args);
    storage(const __m128& reg);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;
};

#include "storage.inl"