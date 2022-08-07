#pragma once

#include <memory>

#include "utils/ComponentInspector.hpp"
#include "utils/Shapes.hpp"

#include "core/definitions.hpp"
#include "core/interpolation.hpp"


const char* const spawnShapeStr[] {"PointShape", "RectangularShape", "CircularShape", "RingShape"};

struct ParticleEmisor{
	std::string texture{""};

	ShapeVariant spawnShape{PointShape()};

	floatVar lifetime{1., 0.};
	floatVar rotation{0., 0.};

	VecInterpolation<floatVar> rotationSpeed{{0., 0.}, {0., 0.}, INTERPOLATION_NULL};
	VecInterpolation<floatVar> size{{1., 0.}, {0., 0.}, INTERPOLATION_NULL};
	VecInterpolation<Vec2VarRotation> speed{{{1., 0.},{0.,0.}}, {{0.,0.},{0.,0.}}, INTERPOLATION_NULL};
	VecInterpolation<Vec4Var> color{{{1., 1., 1., 1.},{0.,0.,0.,0.}}, {{1., 1., 1., 1.},{0.,0.,0.,0.}}, INTERPOLATION_NULL};

	int burstCount{0};
	float continuousGenerationRate{1.};

	bool continuous{false};
	bool burst{false};

	// Auxiliary variables
	float deltaTimeAcc{0.};
};

COMPONENT_INSPECTOR(ParticleEmisor, texture, spawnShape, lifetime, rotation, rotationSpeed, size, speed, color, burstCount, continuousGenerationRate, continuous);
