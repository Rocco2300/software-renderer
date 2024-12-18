#include "math/storage.hpp"

#include <cassert>

template <typename T, size_t Size>
template <typename... Args>
storage<T, Size>::storage(Args&&... args) {
    int counter{};
    for (const auto& arg : std::initializer_list<float>({args...})) {
        auto idx = counter++;
        assert(counter <= Size);

        data[idx] = arg;
    }
}

template <typename T, size_t Size>
storage<T, Size>::storage(const __m128& reg) {
    _mm_store_ps(data, reg);
}

template <typename T, size_t Size>
storage<T, Size>::operator T*() {
    return data;
}

template <typename T, size_t Size>
storage<T, Size>::operator __m128() const {
    return _mm_load_ps(data);
}

template <typename T, size_t Size>
T& storage<T, Size>::operator[](size_t index) {
    return data[index];
}

template <typename T, size_t Size>
const T& storage<T, Size>::operator[](size_t index) const {
    return data[index];
}
