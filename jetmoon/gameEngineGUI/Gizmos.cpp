#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gameEngineGUI/Gizmos.hpp"
#include <GL/glew.h>                   // for GLuint
#include <math.h>                      // for M_PI
#include "ImGuizmo/ImGuizmo.h"         // for BeginFrame, Manipulate, SetGiz...
#include "components/Transform.hpp"    // for Transform, fromMatrix, toMatrix
#include "core/definitions.hpp"        // for Vec3
#include "imgui/imgui.h"               // for IsKeyPressed, GetWindowDrawList


bool Gizmos::renderedGizmos = false;
ImGuizmo::MODE Gizmos::mode = ImGuizmo::WORLD;
glm::mat4 Gizmos::viewTr = {};
float Gizmos::dx = 0;
float Gizmos::dy = 0;
float Gizmos::width = 0;
float Gizmos::height = 0;
float Gizmos::isSnap = false;
float Gizmos::gizmosSize = 1.;


void Gizmos::setState(glm::mat4 viewTr, GLuint dx, GLuint dy, GLuint width, GLuint height){
	Gizmos::viewTr = viewTr;
	Gizmos::dx = dx;
	Gizmos::dy = dy;
	Gizmos::width = width;
	Gizmos::height = height;
	Gizmos::renderedGizmos = false;
}

void Gizmos::renderGizmos(glm::mat4 trans, std::vector<Transform*>& trVector, Vec3 deltaCenter){
	if (Gizmos::renderedGizmos) return;
	ImGuizmo::BeginFrame();
//	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::SetOrthographic(true);
	//ImGuizmo::SetDrawlist();
	ImGuizmo::SetGizmoSizeClipSpace(Gizmos::gizmosSize); 
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	if (ImGui::IsKeyPressed(71))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(82))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(83)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	Transform newTr = *trVector[0];
	newTr.position += deltaCenter;
	auto newTrans = trans * toMatrix(newTr);
	//newTrans = glm::translate(trans, glm::vec3(0., 0., 0.8));
	auto inverse = glm::affineInverse(trans);
	ImGuizmo::SetRect(dx, dy, width, height);

	float snapValue[3] = {0.5, 0.5, 0.5};
	if(mCurrentGizmoOperation == ImGuizmo::ROTATE){
		snapValue[0] = 45.;
		snapValue[1] = 45.;
		snapValue[2] = 45.;
	}

	ImGuizmo::Manipulate(glm::value_ptr(viewTr), glm::value_ptr(glm::mat4(1.0f)), mCurrentGizmoOperation, Gizmos::mode, glm::value_ptr(newTrans), nullptr, isSnap? snapValue: nullptr);
	if(ImGuizmo::IsUsing()){
		newTr  = fromMatrix(inverse * newTrans);
		auto deltaTr = newTr - *trVector[0];
		deltaTr.position -= deltaCenter;
		for(auto* trPointer: trVector){
			*trPointer += deltaTr;
		}
	}

	Gizmos::renderedGizmos = true;
}

//position[3], scale[2], rotation[1]
void Gizmos::renderGizmos2D(float* position, float* scale, float* rotation){
	if (Gizmos::renderedGizmos) return;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(true);
	//ImGuizmo::SetDrawlist();
	ImGuizmo::SetGizmoSizeClipSpace(Gizmos::gizmosSize); 
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	if (ImGui::IsKeyPressed(71))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(82))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(83)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3]{position[0], position[1], position[2]};
	float matrixScale[3]{scale[0], scale[1], 1.f};
	float matrixRotation[3]{0., 0., 360*rotation[0]/(float(2.*M_PI))};
	float matrix[16];
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
	ImGuizmo::SetRect(dx, dy, width, height);

	float snapValue[3] = {0.5, 0.5, 0.5};
	if(mCurrentGizmoOperation == ImGuizmo::ROTATE){
		snapValue[0] = 45.;
		snapValue[1] = 45.;
		snapValue[2] = 45.;
	}

	ImGuizmo::Manipulate(glm::value_ptr(viewTr), glm::value_ptr(glm::mat4(1.0f)), mCurrentGizmoOperation, Gizmos::mode, matrix, nullptr, isSnap? snapValue: nullptr);
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	position[0] = matrixTranslation[0];
	position[1] = matrixTranslation[1];
	position[2] = matrixTranslation[2];
	scale[0] = matrixScale[0];
	scale[1] = matrixScale[1];
	rotation[0] = (matrixRotation[2]*2.*M_PI)/360;

	Gizmos::renderedGizmos = true;
}


void Gizmos::renderGizmos3D(float (&position)[3], float (&scale)[3], float (&rotation)[3]){
	if (Gizmos::renderedGizmos) return;
}
