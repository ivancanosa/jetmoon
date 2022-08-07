#pragma once

#include <memory>

#include "core/definitions.hpp"
#include "utils/ComponentInspector.hpp"

struct SpriteAnimation{
	Vec3 position{0., 0., 0.};
	Vec2 size{1., 1.};
	float rotation{0.};

	std::string animationController{""};
	bool flipX{false};
	bool flipY{false};
	bool visible{true};

	bool overrideDuration{false};
	float duration{1.};

	std::string subAnimation{""};

	//Internal variables
	float deltaTime{0.};
	int row{0};
	int animationFrame{0};
};

COMPONENT_INSPECTOR(SpriteAnimation, position, size, animationController, flipX, flipY, visible, overrideDuration, duration, subAnimation);

