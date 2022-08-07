#pragma once

#include "definitions.hpp"

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
	virtual IComponentArray* clone() = 0;
};
