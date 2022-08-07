#pragma once

#include "core/definitions.hpp"

struct MovementVelocityVector{
	Vec2 vector{0., 0.};
};

COMPONENT_INSPECTOR(MovementVelocityVector, vector);

struct MovementForceVector{
	Vec2 vector{0., 0.};
};
COMPONENT_INSPECTOR(MovementForceVector, vector);


struct MovementVector{
	bool enable{true};
	std::variant<MovementVelocityVector, MovementForceVector> movementVector;
};

COMPONENT_INSPECTOR(MovementVector, enable, movementVector);
