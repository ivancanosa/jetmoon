#pragma once

#include <box2d/box2d.h>

#include "core/definitions.hpp"
#include "utils/ComponentInspector.hpp"
#include "utils/Shapes.hpp"


enum BodyType {Static, Kinematic, Dynamic};
const char* const bodyTypeStr[] {"Static", "Kinematic", "Dynamic"};

/*struct StaticBody{}; struct KinematicBody{}; struct DynamicBody{};
using BodyTypeV = std::variant<StaticBody, KinematicBody, DynamicBody>;*/

struct Fixture{
	Vec2 position{0., 0.};
	float rotation{0.};
	int tag{0};
	bool isSensor{false};
	bool enable{true};
	ShapeVariant shape{PointShape()};
	float density{1.}, friction{0.}, restitution{0.};
	uint16_t collisionGroup{0x0000000000000001};
	uint16_t collisionMask {0x0000000000000001};
};

struct RigidBody2D{
	Vec2 position{0., 0.};
	float rotation{0};
	bool fixedRotation{false};
	BodyType bodyType{BodyType::Static};
	std::vector<Fixture> fixtureVector{Fixture()};

	bool updateBody{false};

	b2Body* b2Body{nullptr};
};

COMPONENT_INSPECTOR(Fixture, position, isSensor, enable, tag, shape, density, friction, restitution, collisionGroup, collisionMask);
COMPONENT_INSPECTOR(RigidBody2D, position, rotation, fixedRotation, bodyType, fixtureVector);
