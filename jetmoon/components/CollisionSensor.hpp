#pragma once

#include "utils/Shapes.hpp"
#include "core/definitions.hpp"

struct CollisionInfo{
	Entity entity{NullEntity};
	Vec2 normal{0., 0.};
	int tag{0};
	uint16_t collisionGroup{};
	uint16_t collisionMask{};
};

struct CollisionSensor{
	ShapeVariant shape;

	std::vector<CollisionInfo> enteredEntities{};
	std::vector<CollisionInfo> leavingEntities{};
	std::vector<CollisionInfo> stayingEntities{};
};

COMPONENT_INSPECTOR(CollisionSensor, shape);
