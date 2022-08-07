#pragma once

#include <memory>
#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"

struct Sprite{
	Vec3 position{0., 0., 0.};
	Vec2 size{1., 1.};
	float rotation{0.};
	float tiling{1.0};
	Color4 color{1., 1., 1., 1.};
	bool emisor{false};

	std::string id{};
	bool flipX{false};
	bool flipY{false};
	bool visible{true};

};

COMPONENT_INSPECTOR(Sprite, position, size, rotation, tiling, color, emisor, id, flipX, flipY, visible);
