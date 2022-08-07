#pragma once
#include <GL/glew.h>                    // for GLuint
#include <vector>                       // for vector
#include "imgui/imgui.h"  
#include "ImGuizmo/ImGuizmo.h"          // for MODE
#include "glm/ext/matrix_float4x4.hpp"  // for mat4
struct Transform;
struct Vec3;


class Gizmos{
public:
	static void setState(glm::mat4 viewTr, GLuint dx, GLuint dy, GLuint width, GLuint height);

	static void renderGizmos(glm::mat4 trans, std::vector<Transform*>& trVector, Vec3 deltaCenter);

	//position[3], scale[2], rotation[1]
	static void renderGizmos2D(float* position, float* scale, float* rotation);


	static void renderGizmos3D(float (&position)[3], float (&scale)[3], float (&rotation)[3]);

	static bool renderedGizmos;
	static glm::mat4 viewTr;
	static float dx, dy, width, height;
	static float isSnap;
	static float gizmosSize;
	static ImGuizmo::MODE mode;
};
