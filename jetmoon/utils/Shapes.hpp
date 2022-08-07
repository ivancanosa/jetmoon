#pragma once

#include <variant>
#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"

struct PointShape{char _{};};
struct RectangularShape{
	float width{1.}, height{1.}, rotation{0.};
};
struct CircularShape{
	float radio{1.};
};
struct RingShape{
	float innerRadio{1.};
	float outerRadio{1.5};
};
struct ChainShape{
	std::vector<Vec2> vertices;
};
struct ConeShape{
	float radio{1.}, aperture{1.};
};


COMPONENT_INSPECTOR(PointShape, _);
COMPONENT_INSPECTOR(RectangularShape, width, height, rotation);
COMPONENT_INSPECTOR(CircularShape, radio);
COMPONENT_INSPECTOR(RingShape, innerRadio, outerRadio);
COMPONENT_INSPECTOR(ConeShape, radio, aperture);
COMPONENT_INSPECTOR(ChainShape, vertices);

using ShapeVariant = std::variant<PointShape, RectangularShape, CircularShape, RingShape, ConeShape, ChainShape>;


