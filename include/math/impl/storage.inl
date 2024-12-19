#include "storage.hpp"

#include <cassert>

//template <typename T, size_t Size>
//template <typename... Args>
//storage<T, Size>::storage(Args&&... args) {
//    int counter{};
//    for (const auto& arg: std::initializer_list<float>({args...})) {
//        auto idx = counter++;
//        assert(counter <= Size);
//
//        data[idx] = arg;
//    }
//}

template <typename T, size_t Size>
storage<T, Size>::storage(const __m128& reg) {
    float buffer[4];
    _mm_storeu_ps(buffer, reg);

    for (int i = 0; i < Size; i++) {
        this->data[i] = buffer[i];
    }
}

template <typename T, size_t Size>
storage<T, Size>::storage(const storage<T, Size>& other) {
    for (int i = 0; i < Size; i++) {
        this->data[i] = other.data[i];
    }
}

template <typename T, size_t Size>
storage<T, Size>& storage<T, Size>::operator=(const __m128& reg) {
    float buffer[4];
    _mm_storeu_ps(buffer, reg);

    for (int i = 0; i < Size; i++) {
        this->data[i] = buffer[i];
    }

    return *this;
}

template <typename T, size_t Size>
storage<T, Size>& storage<T, Size>::operator=(const storage<T, Size>& other) {
    for (int i = 0; i < Size; i++) {
        this->data[i] = other.data[i];
    }
    return *this;
}

template <typename T, size_t Size>
storage<T, Size>::operator T*() {
    return data;
}

template <typename T, size_t Size>
storage<T, Size>::operator __m128() const {
    __m128 reg;
    __m128 mask;
    switch (Size) {
    case 2:
        reg  = _mm_loadu_ps(data);
        mask = _mm_castsi128_ps(_mm_set_epi32(0, 0, -1, -1));
        reg  = _mm_and_ps(reg, mask);
        break;
    case 3:
        reg  = _mm_loadu_ps(data);
        mask = _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1));
        reg  = _mm_and_ps(reg, mask);
        break;
    case 4:
        reg = _mm_loadu_ps(data);
        break;
    }
    return reg;
}

template <typename T, size_t Size>
T& storage<T, Size>::operator[](size_t index) {
    assert(index >= 0 && index < Size);
    return data[index];
}

template <typename T, size_t Size>
const T& storage<T, Size>::operator[](size_t index) const {
    assert(index >= 0 && index < Size);
    return data[index];
}
