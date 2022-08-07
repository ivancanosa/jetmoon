#pragma once

#include <memory>
#include <assert.h>
#include <unordered_set>
#include "utils/ComponentInspector.hpp"

#include "core/definitions.hpp"

//const size_t MAX_CHILDREN = 16;

struct Hierarchy{
	Entity parent{NullEntity};
	std::unordered_set<Entity> children{};
};

COMPONENT_INSPECTOR(Hierarchy, parent, children);
