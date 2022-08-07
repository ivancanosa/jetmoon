#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <math.h>
#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"


struct Transform{
	Vec3 position{0., 0., 0.};
	Vec2 scale{1., 1.};
	float rotation{0.};

	Transform operator -(const Transform& a){
		Transform tr{*this};
		tr.position -= a.position;
		tr.scale -= a.scale;
		tr.rotation -= a.rotation;
		return tr;
	}

	Transform operator +(const Transform& a){
		Transform tr{*this};
		tr.position += a.position;
		tr.scale += a.scale;
		tr.rotation += a.rotation;
		return tr;
	}

	void operator +=(const Transform& a){
		this->position += a.position;
		this->scale += a.scale;
		this->rotation += a.rotation;
	}

};
glm::mat4 toMatrix(Transform& tr);

Transform fromMatrix(glm::mat4 transformation);

COMPONENT_INSPECTOR(Transform, position, scale, rotation);

