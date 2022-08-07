#pragma once

#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"
#include "core/interpolation.hpp"

struct Trail{
	float time{1.};

	VecInterpolation<float> width{0.1, 0., INTERPOLATION_NULL};
	VecInterpolation<Color4> color{{1., 1., 1., 1.}, {1., 1., 1., 0.}, INTERPOLATION_NULL};
	float minVertexDistance{0.1};
	bool autodestruct{true};
	bool renderAsSprites{false};
	std::string texture{""};


	Vec2 firstPoint{0., 0.};
};

COMPONENT_INSPECTOR(Trail, time, width, color, minVertexDistance, autodestruct, renderAsSprites, texture);
