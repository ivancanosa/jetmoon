#pragma once

#include "utils/ComponentInspector.hpp"  // for COMPONENT_INSPECTOR
#include <math.h>                        // for sqrt
#include <assert.h>
#include <stddef.h>                      // for size_t
#include <stdint.h>                      // for uint32_t, uint64_t, uint8_t
#include <bitset>                        // for bitset
#include <string>

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 128;
const Entity MAX_ENTITIES = 8192;
const Entity NullEntity = MAX_ENTITIES + 1;
const float EPSILON = 0.00001f;
using Signature = std::bitset<MAX_COMPONENTS>;

#ifndef M_PI
	const double M_PI = 3.14159265358979323846;
#endif

using UUID = uint64_t;

UUID createUUID();

using Timeline = std::string;

const unsigned int MAX_PARTICLES = 600000;

template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
	return clamp(v, lo, hi, std::less<>());
}

struct Vec2{
	float x, y;

	void normalize(){
		if(x != 0 && y != 0){
			float module = sqrt(x*x + y*y);
			x /= module;
			y /= module;
		}
	}

	float modulo(){
		return sqrt(x*x + y*y);
	}

	float dot(Vec2& a){
		return x * a.x + y * a.y;
	}

	Vec2 getNormalized(){
		float module = sqrt(x*x + y*y);
		return {x/module, y/module};
	}

	Vec2& operator +=(const Vec2& a){
		x += a.x;
		y += a.y;
		return *this;
	}

	Vec2& operator -=(const Vec2& a){
		x -= a.x;
		y -= a.y;
		return *this;
	}

	Vec2& operator *=(const Vec2& a){
		x *= a.x;
		y *= a.y;
		return *this;
	}

	Vec2& operator /=(const Vec2& a){
		x /= a.x;
		y /= a.y;
		return *this;
	}

	Vec2 operator +(const Vec2& a){
		return {x + a.x, y + a.y};
	}

	Vec2 operator -(const Vec2& a){
		return {x - a.x, y - a.y};
	}

	Vec2 operator *(const Vec2& a){
		return {x * a.x, y * a.y};
	}

	Vec2 operator /(const Vec2& a){
		return {x / a.x, y / a.y};
	}

	Vec2 operator +(float a){
		return {x + a, y + a};
	}

	Vec2 operator -(float a){
		return {x - a, y - a};
	}

	Vec2 operator *(float a){
		return {x * a, y * a};
	}

	Vec2 operator /(float a){
		return {x / a, y / a};
	}



	float& operator [](const size_t& i){
		assert(i <= 2 && "Invalid index for Vec2 type");
		if(i == 0)
			return x;
		return y;
	}
};

struct Vec3{
	float x, y, z;

	void normalize(){
		float module = sqrt(x*x + y*y + z*z);
		x /= module;
		y /= module;
		z /= module;
	}

	float dot(Vec3& a){
		return x * a.x + y * a.y + z * a.z;
	}

	Vec3 getNormalized(){
		float module = sqrt(x*x + y*y + z*z);
		return {x/module, y/module, z/module};
	}

	Vec3& operator +=(const Vec3& a){
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Vec3& operator -=(const Vec3& a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Vec3& operator *=(const Vec3& a){
		x *= a.x;
		y *= a.y;
		z *= a.z;
		return *this;
	}

	Vec3& operator /=(const Vec3& a){
		x /= a.x;
		y /= a.y;
		z /= a.z;
		return *this;
	}

	Vec3 operator +(const Vec3& a){
		return {x + a.x, y + a.y, z + a.z};
	}

	Vec3 operator -(const Vec3& a){
		return {x - a.x, y - a.y, z - a.z};
	}

	Vec3 operator *(const Vec3& a){
		return {x * a.x, y * a.y, z * a.z};
	}

	Vec3 operator /(const Vec3& a){
		return {x / a.x, y / a.y, z / a.z};
	}

	Vec3 operator +(float a){
		return {x + a, y + a, z + a};
	}

	Vec3 operator -(float a){
		return {x - a, y - a, z -a};
	}

	Vec3 operator *(float a){
		return {x * a, y * a, z * a};
	}

	Vec3 operator /(float a){
		return {x / a, y / a, z / a};
	}

	float& operator [](const size_t& i){
		assert(i <= 3 && "Invalid index for Vec3 type");
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		}
		return z;
	}
};

struct Vec4{
	float x, y, z, w;

	void normalize(){
		float module = sqrt(x*x + y*y + z*z + w*w);
		x /= module;
		y /= module;
		z /= module;
		w /= module;
	}

	float dot(const Vec4& a){
		return x * a.x + y * a.y + z * a.z;
	}

	Vec4 getNormalized(){
		float module = sqrt(x*x + y*y + z*z + w*w);
		return {x/module, y/module, z/module, w/module};
	}

	Vec4& operator +=(const Vec4& a){
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
		return *this;
	}

	Vec4& operator -=(const Vec4& a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
		return *this;
	}

	Vec4& operator *=(const Vec4& a){
		x *= a.x;
		y *= a.y;
		z *= a.z;
		w *= a.w;
		return *this;
	}

	Vec4& operator /=(const Vec4& a){
		x /= a.x;
		y /= a.y;
		z /= a.z;
		w /= a.w;
		return *this;
	}

	Vec4 operator +(const Vec4& a){
		return {x + a.x, y + a.y, z + a.z, w + a.w};
	}

	Vec4 operator -(const Vec4& a){
		return {x - a.x, y - a.y, z - a.z, w - a.w};
	}

	Vec4 operator *(const Vec4& a){
		return {x * a.x, y * a.y, z * a.z, w * a.w};
	}

	Vec4 operator /(const Vec4& a){
		return {x / a.x, y / a.y, z / a.z, w / a.w};
	}

	Vec4 operator +(float a){
		return {x + a, y + a, z + a, w + a};
	}

	Vec4 operator -(float a){
		return {x - a, y - a, z - a, w - a};
	}

	Vec4 operator *(float a){
		return {x * a, y * a, z * a, w * a};
	}

	Vec4 operator /(float a){
		return {x / a, y / a, z / a, w / a};
	}

	float& operator [](const size_t& i){
		assert(i <= 4 && "Invalid index for Vec4 type");
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return w;
	}
};

struct Color3{
	float x, y, z;

	void normalize(){
		float module = sqrt(x*x + y*y + z*z);
		x /= module;
		y /= module;
		z /= module;
	}

	float dot(Color3& a){
		return x * a.x + y * a.y + z * a.z;
	}

	Color3 getNormalized(){
		float module = sqrt(x*x + y*y + z*z);
		return {x/module, y/module, z/module};
	}

	Color3& operator +=(const Color3& a){
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Color3& operator -=(const Color3& a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Color3& operator *=(const Color3& a){
		x *= a.x;
		y *= a.y;
		z *= a.z;
		return *this;
	}

	Color3& operator /=(const Color3& a){
		x /= a.x;
		y /= a.y;
		z /= a.z;
		return *this;
	}

	Color3 operator +(const Color3& a){
		return {x + a.x, y + a.y, z + a.z};
	}

	Color3 operator -(const Color3& a){
		return {x - a.x, y - a.y, z - a.z};
	}

	Color3 operator *(const Color3& a){
		return {x * a.x, y * a.y, z * a.z};
	}

	Color3 operator /(const Color3& a){
		return {x / a.x, y / a.y, z / a.z};
	}

	Color3 operator +(float a){
		return {x + a, y + a, z + a};
	}

	Color3 operator -(float a){
		return {x - a, y - a, z -a};
	}

	Color3 operator *(float a){
		return {x * a, y * a, z * a};
	}

	Color3 operator /(float a){
		return {x / a, y / a, z / a};
	}

	float& operator [](const size_t& i){
		assert(i <= 3 && "Invalid index for Color3 type");
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		}
		return z;
	}
};

struct Color4{
	float x, y, z, w;

	void normalize(){
		float module = sqrt(x*x + y*y + z*z + w*w);
		x /= module;
		y /= module;
		z /= module;
		w /= module;
	}

	float dot(const Color4& a){
		return x * a.x + y * a.y + z * a.z;
	}

	Color4 getNormalized(){
		float module = sqrt(x*x + y*y + z*z + w*w);
		return {x/module, y/module, z/module, w/module};
	}

	Color4& operator +=(const Color4& a){
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
		return *this;
	}

	Color4& operator -=(const Color4& a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
		return *this;
	}

	Color4& operator *=(const Color4& a){
		x *= a.x;
		y *= a.y;
		z *= a.z;
		w *= a.w;
		return *this;
	}

	Color4& operator /=(const Color4& a){
		x /= a.x;
		y /= a.y;
		z /= a.z;
		w /= a.w;
		return *this;
	}

	Color4 operator +(const Color4& a){
		return {x + a.x, y + a.y, z + a.z, w + a.w};
	}

	Color4 operator -(const Color4& a){
		return {x - a.x, y - a.y, z - a.z, w - a.w};
	}

	Color4 operator *(const Color4& a){
		return {x * a.x, y * a.y, z * a.z, w * a.w};
	}

	Color4 operator /(const Color4& a){
		return {x / a.x, y / a.y, z / a.z, w / a.w};
	}

	Color4 operator +(float a){
		return {x + a, y + a, z + a, w + a};
	}

	Color4 operator -(float a){
		return {x - a, y - a, z - a, w - a};
	}

	Color4 operator *(float a){
		return {x * a, y * a, z * a, w * a};
	}

	Color4 operator /(float a){
		return {x / a, y / a, z / a, w / a};
	}

	float& operator [](const size_t& i){
		assert(i <= 4 && "Invalid index for Color4 type");
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return w;
	}
};


COMPONENT_INSPECTOR(Vec2, x, y);
COMPONENT_INSPECTOR(Vec3, x, y, z);
COMPONENT_INSPECTOR(Vec4, x, y, z, w);
COMPONENT_INSPECTOR(Color3, x, y, z);
COMPONENT_INSPECTOR(Color4, x, y, z, w);
