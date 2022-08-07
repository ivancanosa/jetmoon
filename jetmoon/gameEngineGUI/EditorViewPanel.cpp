#include "EditorViewPanel.hpp"
#include <memory>
#include <array>                           // for array
#include <type_traits>                     // for remove_extent_t
#include <vector>                          // for vector
#include "GL/glew.h"                       // for glGetIntegerv, GLuint, glR...
#include "ImGuizmo/ImGuizmo.h"             // for IsUsing
#include "commonStructures/Camera.hpp"     // for Camera
#include "components/CameraComponent.hpp"  // for CameraComponent
#include "components/Transform.hpp"        // for Transform
#include "core/ServiceContext.hpp"         // for ServiceContext, EditorDS
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext
#include "gameEngineGUI/Gizmos.hpp"        // for Gizmos
#include "imgui/imgui.h"                   // for ImVec2, GetIO, PopStyleVar
#include "imgui/imgui_impl_glfw.h"         // for imgui_viewport, imgui_chan...
#include "opengl/Framebuffer.hpp"          // for Framebuffer, FramebufferDe...
#include "services/RenderContext.hpp"      // for RenderContext
#include "services/EditorDS.hpp"      
//#include "utils/nuklear_glfw_gl4.h"        // for nuklear_viewport, nuklear_...

void EditorViewPanel::setGizmos(Entity entity, std::unordered_set<Entity>* selectedEntities){
	selectedEntity = entity;
	this->selectedEntities = selectedEntities;
}

void EditorViewPanel::renderGizmos(World* world, ServiceContext* serviceContext){
	if(selectedEntity != NullEntity){
		int i = 1;
		auto origin = world->reduceComponent<Transform>(selectedEntity).position;
		Vec3 meanPoint = origin;
		static std::vector<Transform*> trVector{};
		trVector.clear();
		trVector.push_back(&world->getComponent<Transform>(selectedEntity));
		for(auto entity: *selectedEntities){
			if(selectedEntity != entity){
				trVector.push_back(&world->getComponent<Transform>(entity));
				meanPoint += world->reduceComponent<Transform>(entity).position;
				i += 1;
			}
		}
		meanPoint.x = meanPoint.x/i - origin.x;
		meanPoint.y = meanPoint.y/i - origin.y;
		meanPoint.z = meanPoint.z/i - origin.z;
		Gizmos::renderGizmos(world->reduceTransformMatrixParent(selectedEntity), trVector, meanPoint);
	}
}

std::tuple<int, int> EditorViewPanel::spawnRenderTarget(int topWindowHeight, int rightWindowWidth, int bottomWindowHeight, int leftWindowWidth){
	int viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);
	ImVec2 size{(float)viewport[2] - rightWindowWidth - leftWindowWidth, (float)viewport[3] - topWindowHeight - bottomWindowHeight};

	bool isOpen{true};
	int windowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus |  ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;
	ImGui::Begin("WindowRenderTarget", &isOpen, windowFlags);

	ImGui::SetWindowSize(size);
	ImGui::SetWindowPos({(float)leftWindowWidth, (float)topWindowHeight});
	return {size.x, size.y};
}

std::tuple<Entity, bool> EditorViewPanel::getSelectedEntity(int leftMargin, int topMargin){
	//Entity selectedEntity = NullEntity;
	auto[mx, my] = ImGui::GetMousePos();
	mx -= leftMargin;
	my -= topMargin;
	my = targetViewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;
	bool wasEntitySelected{false};
	Entity selectedEntity{NullEntity};
	if(!ImGuizmo::IsUsing() && (mouseX >= 0 && mouseY >= 0 && mouseX < (int)targetViewportSize.x && mouseY < (int)targetViewportSize.y)){
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		int pixelData;
		glReadPixels(mouseX, mouseY, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		if(pixelData >= 1 && ImGui::IsMouseClicked(0)){
			pixelData = pixelData - 1;
			selectedEntity = pixelData;
			wasEntitySelected = true;
		}
	}
	return {selectedEntity, wasEntitySelected};
}

std::tuple<Vec2, Vec2> EditorViewPanel::render(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	Vec2 viewMargin, viewSize;
	std::shared_ptr<Framebuffer> actualRenderTarget;
	Entity activeCamera = worldContext->camera.activeCamera;
	bool fromCamera = false;
	if(serviceContext->editorDS->isPlaying && activeCamera != NullEntity){
		auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
		actualRenderTarget = cameraComponent.framebuffer;
		fromCamera = true;
	}else{
		actualRenderTarget = serviceContext->renderContext->getFramebuffer("editor");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	int flags =  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("View", 0, flags);
		wsize = ImGui::GetWindowSize();
		ImVec2 pos = ImGui::GetWindowPos();
		auto shift = ImGui::GetWindowContentRegionMin();
		viewSize = {wsize.x, wsize.y - shift.y};
		viewMargin = {pos.x + shift.x, pos.y + shift.y};
		ImVec2 padding = {0, 0};
		float ratioOrigin = (float)actualRenderTarget->width / actualRenderTarget->height;
		float ratioDestiny = wsize.x / wsize.y;
		float w, h;
		if(ratioOrigin >= ratioDestiny){
			w = wsize.x;
			h = wsize.y * (ratioDestiny/ratioOrigin);
			padding.y = (wsize.y - h)/2;
		}else{
			h = wsize.y;
			w = wsize.x * (ratioOrigin/ratioDestiny);
			padding.x = (wsize.x - w)/2;
		}
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (wsize.x-w)/2.);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (wsize.y-h)/2.);
		ImGui::Image(reinterpret_cast<ImTextureID>(actualRenderTarget->texturesArray[0]->textureId), {w, h}, ImVec2{0, 1}, ImVec2{1, 0}, tint_col);
		ImGui::PopStyleVar();
		targetViewportSize = ImGui::GetWindowSize();

	renderGizmos(world, serviceContext);
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

//	nuklear_changedViewport = 1;
//	nuklear_viewport[0] = viewMargin.x + padding.x;
//	nuklear_viewport[1] = viewMargin.y + padding.y;
//	nuklear_viewport[2] = actualRenderTarget->texturesArray[0]->width / w;
//	nuklear_viewport[3] = actualRenderTarget->texturesArray[0]->height / h;


	GLint viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);

	imgui_changedViewport = 1;
	imgui_viewport[0] = (viewMargin.x + padding.x);
	imgui_viewport[1] = (viewMargin.y + padding.y);
	imgui_viewport[2] = actualRenderTarget->texturesArray[0]->width / w;
	imgui_viewport[3] = actualRenderTarget->texturesArray[0]->height / h;


	auto[mx, my] = ImGui::GetMousePos();
	mx -= viewMargin.x;
	my -= viewMargin.y;
	my = targetViewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;
	static bool isMouseDragging{false};
	if(mouseX >= 0 && mouseY >= 0 && mouseX < (int)targetViewportSize.x && mouseY < (int)targetViewportSize.y){
		if(!ImGuizmo::IsUsing()){
			if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
				isMouseDragging = true;
			}
			if(ImGui::IsMouseDragging(ImGuiMouseButton_Left) && isMouseDragging){
				float scale = worldContext->camera.scale*4;
				worldContext->camera.position.x -= (ImGui::GetIO().MouseDelta.x/100)/scale;
				worldContext->camera.position.y += (ImGui::GetIO().MouseDelta.y/100)/scale;
			}
			ImGuiIO& io = ImGui::GetIO();
			worldContext->camera.scale += io.MouseWheel/100;
			if(worldContext->camera.scale <= 0.01)
				worldContext->camera.scale = 0.01;
		}
	}
	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
		isMouseDragging = false;
	}

	if(!fromCamera && (wsize.x != actualRenderTarget->width || wsize.y != actualRenderTarget->height)){
		shouldResizeViewFramebuffer = true;
	}

	return {viewMargin, viewSize};
}

void EditorViewPanel::resizeViewFramebuffer(ServiceContext* serviceContext){
	if(!shouldResizeViewFramebuffer) return;
	shouldResizeViewFramebuffer = false;
	FramebufferDefinition fbDef { (GLuint)wsize.x, (GLuint)wsize.y, true, {
		{GL_RGBA, GL_RGBA}, {GL_R32I, GL_RED_INTEGER}
	}};
	auto fb = std::make_shared<Framebuffer>(fbDef);

	serviceContext->renderContext->destroyFramebuffer("editor");
	serviceContext->renderContext->addFramebuffer("editor", fb);
}
