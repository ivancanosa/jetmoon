#pragma once
#include <tuple>                 // for tuple
#include <unordered_set>         // for unordered_set
#include "core/definitions.hpp"  // for Entity, Vec2 (ptr only)
#include "imgui/imgui.h"         // for ImVec2
class World;
struct ServiceContext;
struct WorldContext;


class EditorViewPanel{
public:
	std::tuple<Vec2, Vec2> render(World* world, WorldContext* worldContext, ServiceContext* serviceContext);
	std::tuple<Entity, bool> getSelectedEntity(int leftMargin, int topMargin);
	void setGizmos(Entity entity, std::unordered_set<Entity>* selectedEntities);

	void resizeViewFramebuffer(ServiceContext* serviceContext);


private:
	std::tuple<int, int> spawnRenderTarget(int topWindowHeight, int rightWindowWidth, int bottomWindowHeight, int leftWindowWidth);
	void renderGizmos(World* world, ServiceContext* serviceContext);
	ImVec2 targetViewportSize{};
	ImVec2 wsize;
	bool shouldResizeViewFramebuffer{false};

	std::unordered_set<Entity>* selectedEntities;
	Entity selectedEntity;
};
