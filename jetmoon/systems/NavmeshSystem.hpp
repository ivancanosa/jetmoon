#pragma once
#include <string_view>                    // for string_view
#include <tuple>                          // for tuple
#include <vector>                         // for vector
#include "clipper/clipper.hpp"            // for Paths, Path
#include "core/ISystem.hpp"               // for ISystem
#include "core/definitions.hpp"           // for Vec2 (ptr only), Entity
#include "polypartition/polypartition.h"  // for TPPLPolyList
class Navmesh;
class World;
struct ServiceContext;
struct WorldContext;

class NavmeshSystem: public ISystem{
public:
	void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
	void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	std::string_view getName() override;
	ISystem* clone() override;
	~NavmeshSystem();

private:
	ClipperLib::Paths clipperIntersectPolys(std::vector<std::tuple<bool, ClipperLib::Path>>& originalPolygons, float radius);
	TPPLPolyList tpplPartition(ClipperLib::Paths& clipperPolygons, std::vector<Vec2>& vertexVector);
	void navmeshGenerateGraph(Navmesh& navmesh);

	std::vector<std::tuple<Vec2, Vec2>> edges;
};
