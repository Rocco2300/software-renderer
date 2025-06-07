#pragma once

#include <cmath>

template <typename T>
T rad2Deg(T radians) {
    return radians * static_cast<T>(180) / M_PI;
}

template <typename T>
T deg2Rad(T degrees) {
    return degrees * M_PI / static_cast<T>(180);
}