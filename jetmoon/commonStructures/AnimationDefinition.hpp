#pragma once

#include <unordered_map>
#include <string>
#include <tuple>

#include "utils/MapJsonLoader.hpp"
#include "utils/json.hpp"

struct AnimationDefinition{
	std::string id{""}; //Name of the sprite sheet 
	int margin{0}, padding{0}, frameWidth{16}, frameHeight{16};
//	std::unordered_map<std::string, std::tuple<int, int, bool>> subAnimations{}; //the tuple: (row, final collum, animationDuration, loop?)
	std::vector<std::tuple<std::string, int, int, float, bool>> subAnimations{};
};



//Custom hash specialization
namespace std {
    template<> struct hash<AnimationDefinition> {
        std::size_t operator()(AnimationDefinition const& s) const noexcept {
            return std::hash<std::string>{}(s.id);
        }
    };
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationDefinition, id, margin, padding, frameWidth, frameHeight, subAnimations);
