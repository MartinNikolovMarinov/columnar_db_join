#pragma once

#include <vector>

namespace core {

template <typename T>
void push_back_unique(std::vector<T>& vec, const T& value) {
    if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
        vec.push_back(value); // copy
    }
}

template <typename T>
void push_back_unique(std::vector<T>& vec, T&& value) {
    if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
        vec.push_back(std::move(value));
    }
}

} // namespace core
