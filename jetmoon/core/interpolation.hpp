#pragma once
#include "core/definitions.hpp"
#include "utils/ComponentInspector.hpp"  // for COMPONENT_INSPECTOR

std::function<float(float)> monotoneCubicInterpolation(const std::vector<Vec2>& points);

struct Vec2VarRotation{
	Vec2 origin{};
	Vec2 var{}; // this vector is repesented as modulus and angle
};
template <class T>
struct VecVar{
	T origin{};
	T span{};
};
typedef VecVar<float> floatVar;
typedef VecVar<Vec2> Vec2Var;
typedef VecVar<Vec3> Vec3Var;
typedef VecVar<Vec4> Vec4Var;

float randomNumberBetween(float left, float right);
float randomNumberCentered(float center, float span);
float randomVec(const VecVar<float>& floatVar);
Vec2 randomVec(const Vec2VarRotation& vec2);
Vec4 randomVec(const VecVar<Vec4>& vec4);

enum Interpolation{INTERPOLATION_NULL, INTERPOLATION_LINEAR, 
	INTERPOLATION_SMOOTHSTART2 ,INTERPOLATION_SMOOTHSTART3, INTERPOLATION_SMOOTHSTART4,
	INTERPOLATION_SMOOTHSTOP2 , INTERPOLATION_SMOOTHSTOP3,  INTERPOLATION_SMOOTHSTOP4,
};
const char* const interpolationStr[] {"No interpolation", "Linear", "smooth start 2", "smooth start 3", "smooth start 4",
	"smooth stop 2", "smooth stop 3", "smooth stop 4"};

template <class T>
struct VecInterpolation{
	T origin{};
	T destiny{};
	Interpolation interpolation{INTERPOLATION_NULL};
};

float linearInterpolation(float v1, float v2, float t);
float smoothStart2(float v1, float v2, float t);
float smoothStart3(float v1, float v2, float t);
float smoothStart4(float v1, float v2, float t);
float smoothStop2(float v1, float v2, float t);
float smoothStop3(float v1, float v2, float t);
float smoothStop4(float v1, float v2, float t);

float interpolation(float v1, float v2, float t, Interpolation interpolation);
float interpolate(VecInterpolation<float> vecInter, float t);
Vec2 interpolate(VecInterpolation<Vec2> vecInter, float t);
Vec3 interpolate(VecInterpolation<Vec3> vecInter, float t);
Vec4 interpolate(VecInterpolation<Vec4> vecInter, float t);
Color3 interpolate(VecInterpolation<Color3> vecInter, float t);
Color4 interpolate(VecInterpolation<Color4> vecInter, float t);



COMPONENT_INSPECTOR(Vec2VarRotation, origin, var);
COMPONENT_INSPECTOR(VecVar<float>, origin, span);
COMPONENT_INSPECTOR(VecVar<Vec2>, origin, span);
COMPONENT_INSPECTOR(VecVar<Vec3>, origin, span);
COMPONENT_INSPECTOR(VecVar<Vec4>, origin, span);

COMPONENT_INSPECTOR(VecInterpolation<float>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Vec2>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Vec3>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Vec4>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Color3>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Color4>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<floatVar>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Vec2VarRotation>, origin, destiny, interpolation);
COMPONENT_INSPECTOR(VecInterpolation<Vec4Var>, origin, destiny, interpolation);
