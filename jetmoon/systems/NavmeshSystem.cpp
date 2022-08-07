#include "systems/NavmeshSystem.hpp"
#include <stddef.h>                         // for NULL
#include <iosfwd>                           // for string
#include <list>                             // for __list_iterator, list
#include <set>                              // for set
#include <type_traits>                      // for decay_t
#include <variant>                          // for visit
#include "box2d/b2_body.h"                  // for b2Body, b2_staticBody
#include "box2d/b2_chain_shape.h"           // for b2ChainShape
#include "box2d/b2_circle_shape.h"          // for b2CircleShape
#include "box2d/b2_fixture.h"               // for b2Fixture, b2Filter
#include "box2d/b2_math.h"                  // for b2Vec2
#include "box2d/b2_polygon_shape.h"         // for b2PolygonShape
#include "box2d/b2_settings.h"              // for b2BodyUserData
#include "box2d/b2_shape.h"                 // for b2Shape, b2Shape::e_chain
#include "box2d/b2_world.h"                 // for b2World
#include "clipper/clipper.hpp"              // for Path, Paths, Clipper, Cli...
#include "commonStructures/Navmesh.hpp"     // for Navmesh
#include "components/RigidBody2D.hpp"       // for RigidBody2D, Fixture
#include "components/TilemapComponent.hpp"  // for TilemapComponent
#include "components/Transform.hpp"         // for Transform
#include "core/ServiceContext.hpp"          // for ServiceContext
#include "core/World.hpp"                   // for World
#include "core/WorldContext.hpp"            // for WorldContext, b2WorldDS
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_inverse.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp> 
#include "polypartition/polypartition.h"    // for TPPLPoint, TPPLPoly, TPPL...
#include "services/Renderer2D.hpp"          // for Renderer2D
struct ChainShape;
using namespace ClipperLib;

#include "core/interpolation.hpp"

long long mult =      10000000000;

enum In{
	I_X, I_Y, I_B, I_A, I_L
};

void NavmeshSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void NavmeshSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void NavmeshSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }


#include "opengl/GLdefinitions.hpp"
void NavmeshSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	auto& navmesh = worldContext->navmesh;
	static bool generated = false;

	//Test monotone cubic interpolation
/*	{
		std::vector<Vec2> points{{0., 0.}, {0.1, 0.3}, {0.5, 0.4}, {0.6, 0.8}, {0.9, 0.9}, {1., 1.}};
		float parts = 100;
		auto interpolator = monotoneCubicInterpolation(points);
		for(int i=0; i<parts; i++){
			float x0 = (float)i/parts;
			float x1 = (float)(i+1)/parts;
			glm::vec3 origin = {x0, interpolator(x0), 0.9};
			glm::vec3 destiny = {x1, interpolator(x1), 0.9};
			serviceContext->renderer2D->drawDebugLine(origin, destiny, {1., 0., 0., 1.});
		}
	}*/

	//Test Spline
/*	std::vector<Vec2> points;
	for(auto entity: mEntities){
		auto& tr = world->getComponent<Transform>(entity);
		points.push_back({tr.position.x, tr.position.y});
	}
	GLuint id =serviceContext->imageLoader.load("ribbon");
	if(points.size() >= 3){
		Spline2D spline;
		spline.points = points;
		spline.generate();
		serviceContext->renderer2D.drawSpline(spline, id, 0.3);
		serviceContext->renderer2D.flushSplines();
	}*/

	//Test pathfinding
	/*
	Vec2 p[2];
	int numberPoints = 0;
	for(auto&& entity: this->mEntities){
		auto& rigid = world->getComponent<RigidBody2D>(entity);
		if(rigid.bodyType == BodyType::Dynamic){
			Vec3 entityTr = world->getComponent<Transform>(entity).position;
			p[numberPoints] = {entityTr.x, entityTr.y};
			numberPoints += 1;
			if(numberPoints >= 2) break;
		}
	}
	if(numberPoints >= 2 && generated){
		auto path = navmesh.getPath(p[0], p[1]);
		if(path.size() > 0){
			glm::vec3 x0 = {p[0].x, p[0].y, 0.9};
			glm::vec3 x1 = {path[0].x, path[0].y, 0.9};
			serviceContext->renderer2D.drawDebugLine(x0, x1, {1., 0., 0., 1.});
			for(int i = 0; i < path.size() - 1; i++){
				glm::vec3 origin = {path[i].x, path[i].y, 0.9};
				glm::vec3 destiny = {path[i+1].x, path[i+1].y, 0.9};
				serviceContext->renderer2D->drawDebugLine(origin, destiny, {1., 0., 0., 1.});
			}
		}
	}*/
	/////////////////////

	if(navmesh.render){
		for(auto poly: navmesh.polysVector){
			int vertexCount = poly.size();
			for(int i=0; i<vertexCount; i++){
				Vec2 a = navmesh.verticesVector[poly[i]];
				Vec2 b = navmesh.verticesVector[poly[(i+1)%vertexCount]];
				glm::vec3 origin = {a.x, a.y, 0.8};
				glm::vec3 destiny = {b.x, b.y, 0.8};
				serviceContext->renderer2D->drawDebugLine(origin, destiny, {1., 1., 1., 0.7});
			}
		}
	}
	serviceContext->renderer2D->flushAll();
	
	if(!navmesh.generate) return;
	generated = true;
	edges.clear();
	navmesh.clear();

	bool hasTilemap = false;
	bool firstPoint = true;
	long long minX, minY, maxX, maxY;

	std::vector<std::tuple<bool, Path>> originalPolygons;
	for(auto&& entity: this->mEntities){
		auto& rigidBody = world->getComponent<RigidBody2D>(entity);
		const auto& tr = world->getComponent<Transform>(entity);
		if(world->hasComponent<TilemapComponent>(entity)){
			int fixtureIndex = 0;
			Path originalPolygon;
			auto& tilemapComponent = world->getComponent<TilemapComponent>(entity);
			for(auto& fixture: rigidBody.fixtureVector){
				hasTilemap = true;
				std::visit([&](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, ChainShape>){
						originalPolygon.clear();
						for(auto[x, y]: arg.vertices){
							long long nx = tr.position.x * mult + x * tr.scale.x * mult;
							long long ny = tr.position.y * mult + y * tr.scale.y * mult;
							originalPolygon.push_back({nx, ny});
						}
						originalPolygons.push_back({ tilemapComponent.isFixtureHole[fixtureIndex], std::move(originalPolygon)});
					}
				}, fixture.shape);
				fixtureIndex += 1;
			}
		}
	}

	auto worldScale = worldContext->b2World.worldScale;
	auto b2World = worldContext->b2World.world;

	for (b2Body* body = b2World->GetBodyList(); body; body = body->GetNext()) {
		Entity entity = body->GetUserData().pointer;
		if(world->hasComponent<TilemapComponent>(entity)) continue;				
		Transform tr = world->reduceComponent<Transform>(entity);
		b2Fixture* F = body->GetFixtureList();             
		if(body->GetType() != b2_staticBody) continue;
		while(F != NULL) {
			if((F->GetFilterData().categoryBits & navmesh.collisionGroup) == 0){
				F = F->GetNext();
				continue;
			}
			switch (F->GetType()) {
				case b2Shape::e_circle: {
					b2CircleShape* circle = (b2CircleShape*) F->GetShape();                     
					b2Vec2 v0 = body->GetWorldPoint({0., 0.});
//						serviceContext->renderer2D.drawCircle({v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON}, circle->m_radius/worldScale, circleId);
				}
				break;
				case b2Shape::e_polygon: {
					 b2PolygonShape* poly = (b2PolygonShape*) F->GetShape();
					Path originalPolygon;
					 for(int i=0; i < poly->m_count; i++){
						b2Vec2 v0 = body->GetWorldPoint(poly->m_vertices[i]);	
						glm::vec3 p{v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON};
						long long nx = p.x * mult;
						long long ny = p.y * mult;
						if(!hasTilemap){
							if(firstPoint){
								firstPoint = false;
								minX = nx;
								minY = ny;
								maxX = nx;
								maxY = ny;
							}else{
								if(nx < minX) minX = nx;
								if(nx > maxX) maxX = nx;
								if(ny < minY) minY = ny;
								if(ny > maxY) maxY = ny;
							}
						}
						originalPolygon.push_back({nx, ny});
					 }
					originalPolygons.push_back({true, std::move(originalPolygon)});
				 }
				 break;
				 case b2Shape::e_chain: {
					b2ChainShape* chain = (b2ChainShape*) F->GetShape();
					Path originalPolygon;
					for(int i=0; i < chain->m_count; i++){
						b2Vec2 v0 = body->GetWorldPoint(chain->m_vertices[i]);	
						glm::vec3 p{v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON};
						long long nx = p.x * mult;
						long long ny = p.y * mult;
						if(!hasTilemap){
							if(firstPoint){
								firstPoint = false;
								minX = nx;
								minY = ny;
								maxX = nx;
								maxY = ny;
							}else{
								if(nx < minX) minX = nx;
								if(nx > maxX) maxX = nx;
								if(ny < minY) minY = ny;
								if(ny > maxY) maxY = ny;
							}
						}
						originalPolygon.push_back({nx, ny});
					}
					originalPolygons.push_back({true, std::move(originalPolygon)});
				 }
				 break;
			}
			F = F->GetNext();
		}
	}

	if(!hasTilemap && !firstPoint){
		Path polygon;
		long long r = mult * navmesh.agentRadius * 10;
		polygon.push_back({minX - r, minY - r});
		polygon.push_back({maxX + r, minY - r});
		polygon.push_back({maxX + r, maxY + r});
		polygon.push_back({minX - r, maxY + r});
		originalPolygons.push_back({false, std::move(polygon)});
	}

	//==================================
	auto clipperPolygons = clipperIntersectPolys(originalPolygons, navmesh.agentRadius);
	auto tpplPolygons = tpplPartition(clipperPolygons, navmesh.verticesVector);

	for(auto& poly: tpplPolygons){
		Vec2 center{0., 0.};
		int numVertices = 0;
		auto* p = poly.GetPoints();	
		int max = poly.GetNumPoints();
		navmesh.polysVector.push_back({});
		for(int i=0; i<poly.GetNumPoints(); i++){
			edges.push_back({{(float)p[i].x, (float)p[i].y}, {(float)p[(i+1)%max].x, (float)p[(i+1)%max].y}});
			center += {(float)p[i].x, (float)p[i].y};
			navmesh.polysVector.back().push_back(p[i].id);
			numVertices += 1;
		}
		center = center / numVertices;
		navmesh.centersVector.push_back(center);
	}

	navmeshGenerateGraph(navmesh);
}

void NavmeshSystem::navmeshGenerateGraph(Navmesh& navmesh){
	int numPolys = navmesh.polysVector.size();
	navmesh.graph.resize(numPolys);
	bool areAdjacent = false;
	int numAdjacentVertex = 0;
	Vec2 vert[2];
	for(int graph0 = 0; graph0 < numPolys - 1; graph0++){
		for(int graph1 = graph0 + 1; graph1 < numPolys; graph1++){
			numAdjacentVertex = 0;
			areAdjacent = false;
			for(int v0: navmesh.polysVector[graph0]){
				for(int v1: navmesh.polysVector[graph1]){
					if(v0 == v1){
						vert[numAdjacentVertex] = navmesh.verticesVector[v0];
						numAdjacentVertex += 1;
						if(numAdjacentVertex >= 2) areAdjacent = true;
						break;
					}
				}
				if(areAdjacent){
					navmesh.graph[graph0].push_back({graph1, vert[1], vert[0]});
					navmesh.graph[graph1].push_back({graph0, vert[0], vert[1]});
					break;
				}
			}
		}
	}
}

Paths NavmeshSystem::clipperIntersectPolys(std::vector<std::tuple<bool, Path>>& originalPolygons, float radius){
	Paths holes, unitedHoles, walkable, solution;
	for(auto&[isHole, poly]: originalPolygons){
		Paths solution;
		ClipperOffset clipperOffset;
		clipperOffset.AddPath(poly, jtSquare, etClosedPolygon);
		if(isHole){
			clipperOffset.Execute(solution, +mult*radius);
		}else{
			clipperOffset.Execute(solution, -mult*radius);
		}
		for(auto& clipperPoly: solution){
			if(isHole){
				holes.push_back(clipperPoly);
			}else{
				walkable.push_back(clipperPoly);
			}
		}
	}
	Clipper clipperHoles, clipperWalkable;
	for(auto& poly: holes){
		clipperHoles.AddPath(poly, ptSubject, true);
	}
	clipperHoles.Execute(ctUnion , unitedHoles, pftNonZero);

	for(auto& poly: walkable){
		clipperWalkable.AddPath(poly, ptSubject, true);
	}
	for(auto& poly: unitedHoles){
		clipperWalkable.AddPath(poly, ptClip, true);
	}
	clipperWalkable.Execute(ctDifference , solution);

	return solution;
}

TPPLPolyList NavmeshSystem::tpplPartition(Paths& clipperPolygons, std::vector<Vec2>& vertexVector){
	TPPLPartition partition{};
	TPPLPolyList polyList, polyWithoutHoles, result;
	int vertexIndex = 0;
	for(auto& clipperPoly: clipperPolygons){
		TPPLPoly tpplPoly;
		tpplPoly.Init(clipperPoly.size());
		int subVertexIndex = 0;

		int polySize = clipperPoly.size();
		for(int i=0; i < polySize; i++){
			auto[x, y] = clipperPoly[i % polySize];
			float xf = (float)x / mult;
			float yf = (float)y / mult;
			tpplPoly[subVertexIndex].x = xf;
			tpplPoly[subVertexIndex].y = yf;
			tpplPoly[subVertexIndex].id = vertexIndex;
			vertexVector.push_back({xf, yf});
			vertexIndex += 1;
			subVertexIndex += 1;
		}
		if(tpplPoly.GetOrientation() == TPPL_ORIENTATION_CW){
			tpplPoly.SetHole(true);
		}else{
			tpplPoly.SetHole(false);
		}
		polyList.push_back(tpplPoly);
	}

	partition.RemoveHoles(&polyList, &polyWithoutHoles);
	partition.ConvexPartition_HM(&polyWithoutHoles, &result);
//	partition.Triangulate_EC(&polyWithoutHoles, &result);

	return result;
}


std::string_view NavmeshSystem::getName(){
	static std::string str{"NavmeshSystem"};
	return str;
}

ISystem* NavmeshSystem::clone(){
	return new NavmeshSystem();
}

NavmeshSystem::~NavmeshSystem(){ }

