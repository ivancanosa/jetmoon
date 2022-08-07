#pragma once

#include <variant>
#include <cassert>

template<typename V, std::size_t N = std::variant_size_v<V>>
struct variant_by_index {
    V make_default(std::size_t i) {
		assert(i < std::variant_size_v<V> && "bad type index");
        constexpr size_t index = std::variant_size_v<V> - N;
        if (i == index) {
            return std::variant_alternative_t<index, V>();
        } else {
            return variant_by_index<V, N - 1>().make_default(i);
        }
    }
};
template<typename V>
struct variant_by_index<V, 0> {
    V make_default(std::size_t i) {
		assert(false && "bad type index");
    }
};
