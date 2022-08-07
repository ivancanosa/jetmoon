#pragma once
#include "glm/ext/matrix_float4x4.hpp"  // for mat4
#include "utils/json.hpp"               // for basic_json, NLOHMANN_DEFINE_T...
#include "core/definitions.hpp"

class Camera{
public:
	glm::mat4 getTransformationMatrix();
	Vec3 worldSpaceToScreenSpace(const Vec3& origin);
	Vec3 worldSpaceToScreenSpace(const glm::mat4& tr);
	Vec2 worldSpaceToScreenPixelSpace(const Vec3& origin);
	Vec2 worldSpaceToScreenPixelSpace(const glm::mat4& tr);
	void updateViewTransform();

	Vec3 position{0. ,0., 0.};
	float scale{0.5};
	float rotation{0.};
	float ratioX{1.};
	Entity activeCamera{NullEntity};
	glm::mat4 viewTransform{};
private:
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Camera, position, scale, activeCamera);
