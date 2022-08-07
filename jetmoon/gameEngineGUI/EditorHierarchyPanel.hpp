#pragma once
#include <unordered_set>         // for unordered_set
#include "core/definitions.hpp"  // for Entity, NullEntity
class World;
struct WorldContext;


class EditorHierarchyPanel{
public:
	void setEntity(Entity entity);
	Entity render(World* world, WorldContext* worldContext);

	std::unordered_set<Entity> selectedEntities{};
private:
	void renderRootEntity(World* world, WorldContext* worldContext, Entity entity, int& node_clicked, int& i);
	bool renderEntity(World* world, WorldContext* worldContext, Entity entity, int& node_clicked, int& i, int node_flags);

	Entity actualEntity{NullEntity};
	int actualLevel = 0;
	int selectedEntityLevel = 0;

	bool rangeSelection{false};
	bool endRangeSelection{false};
	int rangeSelectionLevel{0};
	bool selectAll{false}, collapse{false};
};
