#pragma once

#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"

struct LightSource{
	float radius{1.};
	float intensity{1.};
	Color3 color{1., 1., 1.};
};

COMPONENT_INSPECTOR(LightSource, radius, intensity, color);
