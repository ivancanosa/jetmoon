#include "DebugPhysics.hpp"
#include <stddef.h>                   // for NULL
#include <iostream>                   // for string
#include "GL/glew.h"                  // for GLuint
#include "box2d/b2_body.h"            // for b2Body
#include "box2d/b2_chain_shape.h"     // for b2ChainShape
#include "box2d/b2_circle_shape.h"    // for b2CircleShape
#include "box2d/b2_fixture.h"         // for b2Fixture
#include "box2d/b2_math.h"            // for b2Vec2
#include "box2d/b2_polygon_shape.h"   // for b2PolygonShape
#include "box2d/b2_settings.h"        // for b2BodyUserData
#include "box2d/b2_shape.h"           // for b2Shape, b2Shape::e_chain, b2Sh...
#include "box2d/b2_world.h"           // for b2World
#include "components/Transform.hpp"   // for Transform
#include "core/ServiceContext.hpp"    // for ServiceContext
#include "core/World.hpp"             // for World
#include "core/WorldContext.hpp"      // for WorldContext, b2WorldDS
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "services/ImageLoader.hpp"   // for ImageLoader
#include "services/Renderer2D.hpp"    // for Renderer2D

ISystem* DebugPhysics::clone(){
	return new DebugPhysics();
}

std::string_view DebugPhysics::getName(){
	static std::string str{"DebugPhysics"};
	return str;
}

void DebugPhysics::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void DebugPhysics::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){}

void DebugPhysics::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void DebugPhysics::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto worldScale = worldContext->b2World.worldScale;
	constexpr glm::vec4 magenta{255, 0, 255, 255};
	auto b2World = worldContext->b2World.world;

	GLuint circleId = serviceContext->imageLoader->load("circle");

	//TODO: draw a different color if the fixture is a sensor
	if(worldContext->b2World.showColliders){
		for (b2Body* body = b2World->GetBodyList(); body; body = body->GetNext()) {
			Entity entity = body->GetUserData().pointer;
			Transform tr = world->reduceComponent<Transform>(entity);
			b2Fixture* F = body->GetFixtureList();             
			while(F != NULL) {
				switch (F->GetType()) {
					case b2Shape::e_circle: {
						b2CircleShape* circle = (b2CircleShape*) F->GetShape();                     
						b2Vec2 v0 = body->GetWorldPoint({0., 0.});
						serviceContext->renderer2D->drawCircle({v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON}, circle->m_radius/worldScale, circleId);
					}
					break;
					case b2Shape::e_polygon: {
						 b2PolygonShape* poly = (b2PolygonShape*) F->GetShape();
						 for(int i=0; i < poly->m_count; i++){
							 b2Vec2 v0 = body->GetWorldPoint(poly->m_vertices[i]);	
							 b2Vec2 v1 = body->GetWorldPoint(poly->m_vertices[(i+1) % poly->m_count]);	
							 glm::vec3 origin{v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON};
							 glm::vec3 destiny{v1.x/worldScale, v1.y/worldScale, tr.position.z - EPSILON};
							 serviceContext->renderer2D->drawDebugLine(origin, destiny, magenta);
						 }
					 }
					 break;
					 case b2Shape::e_chain: {
						 b2ChainShape* chain = (b2ChainShape*) F->GetShape();
						 for(int i=0; i < chain->m_count; i++){
							 b2Vec2 v0 = body->GetWorldPoint(chain->m_vertices[i]);	
							 b2Vec2 v1 = body->GetWorldPoint(chain->m_vertices[(i+1) % chain->m_count]);	
							 glm::vec3 origin{v0.x/worldScale, v0.y/worldScale, tr.position.z - EPSILON};
							 glm::vec3 destiny{v1.x/worldScale, v1.y/worldScale, tr.position.z - EPSILON};
							 serviceContext->renderer2D->drawDebugLine(origin, destiny, magenta);
						 }
					 }
				     break;
				}
				F = F->GetNext();
			}
		}
	}
	serviceContext->renderer2D->flushDebugLines();
	serviceContext->renderer2D->flushSprites();
}

DebugPhysics::~DebugPhysics(){
}

