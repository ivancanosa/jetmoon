#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "DebugGUI.hpp"
#include <GL/glew.h>                                // for glGetIntegerv, GLint
#include <array>                                    // for array
#include <set>                                      // for set
#include <string>                                   // for basic_string
#include <tuple>                                    // for tuple
#include <type_traits>                              // for remove_extent_t
#include <vector>                                   // for vector
#include "box2d/b2_math.h"                          // for b2Vec2
#include "commonStructures/Camera.hpp"              // for Camera
#include "commonStructures/EditorConfig.hpp"        // for EditorConfig
#include "commonStructures/LightningConfig.hpp"     // for LightningConfig
#include "commonStructures/Navmesh.hpp"             // for Navmesh
#include "components/CameraComponent.hpp"           // for CameraComponent
#include "components/Transform.hpp"                 // for Transform
#include "core/ServiceContext.hpp"                  // for ServiceContext
#include "core/World.hpp"                           // for World
#include "core/WorldContext.hpp"                    // for WorldContext, b2W...
#include "gameEngineGUI/EngineStatisticsImgui.hpp"  // for printEngineStatis...
#include "gameEngineGUI/Gizmos.hpp"                 // for Gizmos, Gizmos::g...
#include "gameEngineGUI/imguiComponents.hpp"        // for imguiCollisionFil...
#include "imgui/imgui.h"                            // for ImVec4, Checkbox
#include "imgui/imgui_impl_glfw.h"                  // for ImGui_ImplGlfw_Ne...
#include "imgui/imgui_impl_opengl3.h"               // for ImGui_ImplOpenGL3...
#include "opengl/Framebuffer.hpp"                   // for Framebuffer, Fram...
#include "services/ImguiService.hpp"                // for ImguiService
#include "services/RenderContext.hpp"               // for RenderContext
#include "services/Renderer2D.hpp"                  // for Renderer2D
#include "services/EditorDS.hpp"


void DebugGUI::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	ctx = ImGui::CreateContext(serviceContext->imguiService->fontAtlas);
	ImGui::SetCurrentContext(ctx);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	for(auto timeline: serviceContext->editorDS->disabledTimelines){
		world->setTimelineState(timeline, false);
	}

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &windowFramebuffer);
	GLuint width = 1920;
	GLuint height = 1080;

	FramebufferDefinition fbDef { width, height, true, {
		{GL_RGBA, GL_RGBA}, {GL_R32I, GL_RED_INTEGER}
	}};
	auto fb = std::make_shared<Framebuffer>(fbDef);

	serviceContext->renderContext->addFramebuffer("editor", fb);
	serviceContext->renderContext->changeFramebuffer("editor");

	auto window = serviceContext->renderContext->window;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
 //   ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup panels
	serviceContext->editorDS->editorConfig.load();
	serviceContext->editorDS->isPlaying = false;


	///====== Style ========
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//	io.FontDefault = io.Fonts->AddFontFromFileTTF("data/fonts/openSans/static/OpenSans/OpenSans-Medium.ttf", 18.0f);
	io.FontDefault = serviceContext->imguiService->getFont("openSans", 18);
//	serviceContext->editorDS.fontAtlas = io.Fonts;

	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};
	
	//Headers
	colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
	colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
	colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	
	//Buttons
	colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
	colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
	colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
	colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
	colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
	colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
	colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.95f, 0.1505f, 0.1951f, 1.0f};

//	ImGui_ImplOpenGL3_NewFrame();
}

void imguiWorldContext(World* world, WorldContext* worldContext){
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if(ImGui::TreeNode("Camera")){
		auto& camera = worldContext->camera;
		imguiVec3("Position", camera.position, "%.3f", 0);
		ImGui::SliderAngle("Rotation", &camera.rotation, 0., 360.);
		ImGui::InputFloat("Scale", &camera.scale);
		int a = camera.activeCamera;
		ImGui::InputInt("ActiveCamera", &a, 0, 0);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityPayload")){
				IM_ASSERT(payload->DataSize == sizeof(Entity));
				Entity payload_n = *(const Entity*)payload->Data;
				camera.activeCamera = payload_n;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::TreePop();
	}

	if(ImGui::TreeNode("b2World")){
		auto& b2World = worldContext->b2World;
		float gravity[2] = {b2World.gravity.x, b2World.gravity.y};
		ImGui::InputFloat2("Gravity(x,y)", gravity);
		b2World.gravity.x = gravity[0];
		b2World.gravity.y = gravity[1];
		ImGui::InputFloat("worldScale", &worldContext->b2World.worldScale);
		ImGui::Checkbox("Show colliders", &b2World.showColliders);
		ImGui::TreePop();
	}
	if(ImGui::TreeNode("Lightning")){
		auto& lightningConfig = worldContext->lightningConfig;
		ImGui::Checkbox("LightMask", &lightningConfig.lightMask);
		ImGui::ColorEdit3("BackgroundColor", &lightningConfig.backgroundColor.x, 0);
		ImGui::TreePop();
	}
	if(ImGui::TreeNode("Navmesh")){
		auto& navmesh = worldContext->navmesh;
		ImGui::Checkbox("Generate", &navmesh.generate);
		ImGui::Checkbox("Render", &navmesh.render);
		ImGui::InputFloat("Agent Radius", &navmesh.agentRadius);
		imguiCollisionFiltering("Collision Group", &navmesh.collisionGroup, nullptr, 0);
		ImGui::TreePop();
	}
}

void DebugGUI::renderCameraFrame(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	glm::vec4 white{1.0, 1.0, 1.0, 0.7};
	for(auto&& entity: this->mEntities){
		auto& cameraComponent = world->getComponent<CameraComponent>(entity);
		auto tr = world->reduceComponent<Transform>(entity);
		float s = 1/cameraComponent.scale;
		float ratioX = s * ((float)cameraComponent.framebuffer->width / cameraComponent.framebuffer->height);
		float ratioY = s;
		glm::vec4 p0 = {-ratioX+tr.position.x, ratioY+tr.position.y, -0.99, 1.};
		glm::vec4 p1 = {ratioX+tr.position.x, ratioY+tr.position.y, -0.99, 1.};
		glm::vec4 p2 = {ratioX+tr.position.x, -ratioY+tr.position.y, -0.99, 1.};
		glm::vec4 p3 = {-ratioX+tr.position.x, -ratioY+tr.position.y, -0.99, 1.};
		serviceContext->renderer2D->drawDebugLine(p0, p1, white);
		serviceContext->renderer2D->drawDebugLine(p1, p2, white);
		serviceContext->renderer2D->drawDebugLine(p2, p3, white);
		serviceContext->renderer2D->drawDebugLine(p3, p0, white);
	}
}

void DebugGUI::renderGrid(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	if(!serviceContext->editorDS->renderGrid) return;
	glm::vec4 white{1.0, 1.0, 1.0, 0.5};
	float posX = worldContext->camera.position.x;
	float posY = worldContext->camera.position.y;
	float scale = 1./worldContext->camera.scale;
	float range = 1.8;
	float minX = posX - range*scale, maxX = posX + range*scale, minY = posY - range*scale, maxY = posY + range*scale;
//	int delta = std::max(1, int(scale*range) % 10 - 1);
	for(int x = minX; x < maxX; x += 1){
		glm::vec3 p0 = {x, minY, 0.};
		glm::vec3 p1 = {x, maxY, 0.};
		serviceContext->renderer2D->drawDebugLine(p0, p1, white);
	}
	for(int y = minY; y < maxY; y += 1){
		glm::vec3 p0 = {minX, y, 0.};
		glm::vec3 p1 = {maxX, y, 0.};
		serviceContext->renderer2D->drawDebugLine(p0, p1, white);
	}
	serviceContext->renderer2D->flushDebugLines();
}

void DebugGUI::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	imgui_changedViewport = 0;
	ImGui::SetCurrentContext(ctx);

	serviceContext->editorDS->reload = false;
	serviceContext->renderContext->actualFramebuffer->texturesArray[1]->copy(serviceContext->editorDS->entityMapTexture);


	renderCameraFrame(world, worldContext, serviceContext);
	renderGrid(world, worldContext, serviceContext);

	GLint viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);

	auto[selectedEntity, wasEntitySelected] = editorViewPanel.getSelectedEntity(viewMargin.x, viewMargin.y);
	if(wasEntitySelected){
		editorHierarchyPanel.setEntity(selectedEntity);
	}

	serviceContext->renderContext->changeFramebuffer("window");


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->newFrame();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	//====================================
	Gizmos::isSnap = io.KeyCtrl;
	Gizmos::setState(worldContext->camera.getTransformationMatrix(), viewMargin.x, viewMargin.y, viewport[2], viewport[3]);
	Gizmos::gizmosSize = 1/(4*worldContext->camera.scale);

	auto reset = editorTopPanel.render(world, worldContext, serviceContext, 50, 50);
	if(reset){
		editorHierarchyPanel.setEntity(NullEntity);
	}

	editorSubTopPanel.render(world, serviceContext);

	ImGui::Begin("World Context");
	if (ImGui::BeginTabBar("Tabsss", 0)) {
		if (ImGui::BeginTabItem("WorldContext")) {
			imguiWorldContext(world, worldContext);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("ConfigDS")) {
			imguiComponent(world, serviceContext, *serviceContext->configDS, 0);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Statistics")) {
			printEngineStatisticsImgui(serviceContext->engineStatistics.get());
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();

	Entity entity = editorHierarchyPanel.render(world, worldContext);
	editorComponentPanel.render(world, serviceContext, entity);
	editorFileExplorerPanel.render(world, serviceContext);

	editorViewPanel.setGizmos(entity, &editorHierarchyPanel.selectedEntities);
	auto [margin, size] = editorViewPanel.render(world, worldContext, serviceContext);
	viewMargin = margin;
	viewSize = size;

	this->render();
	editorViewPanel.resizeViewFramebuffer(serviceContext);
	serviceContext->renderContext->changeFramebuffer("editor");
	glEnable(GL_DEPTH_TEST);
}

void DebugGUI::newFrame(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DebugGUI::render(){
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ISystem* DebugGUI::clone(){
	auto system = new DebugGUI();
	system->ctx = ctx;
	return system;
}

DebugGUI::~DebugGUI(){

}
