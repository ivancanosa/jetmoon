#include "EditorHierarchyPanel.hpp"
#include <iosfwd>                                  // for string
#include <set>                                     // for set
#include <string>                                  // for basic_string<>::va...
#include "commonStructures/Camera.hpp"             // for Camera
#include "components/Hierarchy.hpp"                // for Hierarchy
#include "components/NameComponent.hpp"            // for NameComponent
#include "components/Transform.hpp"                // for Transform
#include "core/SceneSerializer.hpp"                // for SceneSerializer
#include "core/World.hpp"                          // for World
#include "core/WorldContext.hpp"                   // for WorldContext
#include "gameEngineGUI/imguiPayloadDefinition.h"  // for entityPayload, pre...
#include "imgui/imgui.h"                           // for IsKeyPressed, GetIO

void EditorHierarchyPanel::setEntity(Entity entity){
	ImGuiIO& io = ImGui::GetIO();
	if(io.KeyCtrl){
		actualEntity = entity;
		selectedEntities.insert(entity);
	}else{
		selectedEntities.clear();
		actualEntity = entity;
		selectedEntities.insert(entity);
	}

}

bool EditorHierarchyPanel::renderEntity(World* world, WorldContext* worldContext, Entity entity, int& node_clicked, int& i, int node_flags){
	Entity entityFromPayload{0};
	i += 1;
	std::string name{};
	if(world->hasComponent<NameComponent>(entity)){
		name = world->getComponent<NameComponent>(entity).name + "##" + std::to_string(entity);
	}else{
		name = std::to_string(entity) + "##" + std::to_string(entity);
	}

	bool is_selected = selectedEntities.find(entity) != selectedEntities.end();
	if (is_selected){
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGuiIO& io = ImGui::GetIO();

	if(collapse) ImGui::SetNextItemOpen(false);

	bool node_open = ImGui::TreeNodeEx(name.c_str(), node_flags);
	if(ImGui::IsItemClicked()){
		ImGuiIO& io = ImGui::GetIO();
		if(!io.KeyCtrl && !rangeSelection){
			selectedEntities.clear();
		}
		actualEntity = entity;
		selectedEntities.insert(entity);
		rangeSelection = false;
		rangeSelectionLevel = 0;
		endRangeSelection = true;
	}

	if(selectAll || (rangeSelection && actualLevel == rangeSelectionLevel)){
		selectedEntities.insert(entity);
	}
	

	if(io.KeyShift && actualEntity == entity && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !endRangeSelection){
		rangeSelection = true;
		rangeSelectionLevel = actualLevel;
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		auto tr = world->getComponent<Transform>(entity);
		worldContext->camera.position.x = tr.position.x;
		worldContext->camera.position.y = tr.position.y;
	}

	//Drag and drop entity to another.
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		entityFromPayload = entity;
		ImGui::SetDragDropPayload(entityPayload.c_str(), &entityFromPayload, sizeof(Entity));
		ImGui::Text("%s", std::to_string(entityFromPayload).c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(entityPayload.c_str())){
			IM_ASSERT(payload->DataSize == sizeof(Entity));
			//Entity payload_n = *(const Entity*)payload->Data;
			//world->addChild(entity, payload_n);
			for(auto childEntity: selectedEntities){
				world->addChild(entity, childEntity);
			}
		}
		ImGui::EndDragDropTarget();
	}
	if(actualEntity == entity){
		selectedEntityLevel = actualLevel;
	}

	return node_open;
}

void EditorHierarchyPanel::renderRootEntity(World* world, WorldContext* worldContext, Entity entity, int& node_clicked, int& i){
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	auto& h = world->getComponent<Hierarchy>(entity);
	if(h.children.size() == 0){
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	bool open = this->renderEntity(world, worldContext, entity, node_clicked, i, flags);
	if(open){
		actualLevel += 1;
		for(auto childEntity: h.children){
			this->renderRootEntity(world, worldContext, childEntity, node_clicked, i);
		}
		ImGui::TreePop();
		actualLevel -= 1;
	}
}

Entity EditorHierarchyPanel::render(World* world, WorldContext* worldContext){
	auto& entities = world->getRootEntitiesSet();
	int node_clicked{-1};
	int i=0;

	/*
	   if(resetScene){
	   previousEntity = NullEntity;
	   selection_mask = 0;
	   }*/

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Entity Hierarchy");
	ImGui::BeginChild("Entity Hierarchy child");
	if(ImGui::Button("Create Entity")){
		auto entity = world->createEntity();
		world->addComponent<NameComponent>(entity, {});
	}
	ImGui::SameLine();
	if(ImGui::Button("Collapse all")){
		collapse = true;
	}
	ImGui::SameLine();
	ImGui::SameLine();
	if(ImGui::IsWindowHovered() && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D)){
		for(auto entity: selectedEntities){
			SceneSerializer serializer;
			Entity duplicate = serializer.duplicateEntity(world, entity);
			if(world->hasComponent<NameComponent>(duplicate)){
				auto& name = world->getComponent<NameComponent>(duplicate);
				name.name = name.name + "_Dup";
			}
		}
	}

	if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))){
		for(auto entity: selectedEntities){
			world->destroyEntity(entity);
		}
		actualEntity = NullEntity;
		selectedEntities.clear();
	}

	if(ImGui::IsWindowHovered() && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A)){
		selectAll = true;
	}

	if(ImGui::IsWindowHovered() && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_U)){ //U
		for(auto entity: selectedEntities){
			world->disownEntity(entity);
		}
	}

	if(ImGui::IsWindowHovered() && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G) && selectedEntities.size() >= 2){ //G
		Entity groupEntity = world->createEntity();
		world->addComponent<NameComponent>(groupEntity, {"group"});
		for(auto entity: selectedEntities){
			world->addChild(groupEntity, entity);
		}
	}


	ImGui::Separator();

	for(Entity entity: entities){
		actualLevel = 0;
		this->renderRootEntity(world, worldContext, entity, node_clicked, i);
		actualLevel = 0;
	}

	ImGui::EndChild();
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(prefabPayload.c_str())){
			std::string payload_n = (char*)payload->Data;
			SceneSerializer serializer;
			Entity entity = serializer.loadPrefab(world, payload_n);
			auto& tr = world->getComponent<Transform>(entity);
			tr.position.x = worldContext->camera.position.x;
			tr.position.y = worldContext->camera.position.y;
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::End();


	selectAll = false;
	rangeSelection = false;
	endRangeSelection = false;
	collapse = false;
	return actualEntity;
}
