#pragma once
#include <type_traits>
#include "utils/json.hpp"

template <typename T>
using has_texture_t = decltype(std::declval<T>().textureIndex);

template <typename T>
using has_texture = nlohmann::detail::is_detected<has_texture_t, T>;
