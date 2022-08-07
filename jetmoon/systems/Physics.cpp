#include "Physics.hpp"
#include <math.h>                          // for cos, sin
#include <stddef.h>                        // for NULL
#include <iostream>                        // for string
#include <set>                             // for set
#include <type_traits>                     // for decay_t
#include <variant>                         // for visit
#include <vector>                          // for vector
#include "box2d/b2_chain_shape.h"          // for b2ChainShape
#include "box2d/b2_circle_shape.h"         // for b2CircleShape
#include "box2d/b2_contact.h"              // for b2Contact
#include "box2d/b2_fixture.h"              // for b2Fixture, b2Filter, b2Fix...
#include "box2d/b2_math.h"                 // for b2Vec2, operator-, b2Trans...
#include "box2d/b2_polygon_shape.h"        // for b2PolygonShape
#include "box2d/b2_settings.h"             // for b2BodyUserData, b2_maxPoly...
#include "box2d/b2_world.h"                // for b2World
#include "box2d/b2_world_callbacks.h"      // for b2ContactFilter, b2Contact...
#include "components/CollisionSensor.hpp"  // for CollisionInfo, CollisionSe...
#include "components/MovementVector.hpp"   // for MovementVector, MovementFo...
#include "components/RigidBody2D.hpp"      // for RigidBody2D, Fixture, Body...
#include "components/Transform.hpp"        // for Transform
#include "core/ServiceContext.hpp"         // for EditorDS, ServiceContext
#include "core/Time.hpp"                   // for Time, Time::deltaTime
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext, b2WorldDS
#include "services/EditorDS.hpp"
struct ChainShape;
struct CircularShape;
struct ConeShape;
struct PointShape;
struct RectangularShape;
struct RingShape;


class CustomContactFilter: public b2ContactFilter{
  bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override{
	  return fixtureA->enable && fixtureB->enable;
  }
};

class CollisionListener: public b2ContactListener {
public:
	World* world;

	void BeginContact(b2Contact* contact) override{
		if (contact && contact->IsTouching()) {
			auto* fixtureA = (contact->GetFixtureA());
			auto* fixtureB = (contact->GetFixtureB());
			Entity entityA = static_cast<Entity>(fixtureA->GetBody()->GetUserData().pointer);
			Entity entityB = static_cast<Entity>(fixtureB->GetBody()->GetUserData().pointer);

			b2Vec2 posA = fixtureA->GetBody()->GetTransform().p;
			b2Vec2 posB = fixtureB->GetBody()->GetTransform().p;
			b2Vec2 normal = posA - posB;
			Vec2 normalVec{normal.x, normal.y};

			if(world->hasComponent<CollisionSensor>(entityA) && !fixtureB->IsSensor()){
				int tag = fixtureA->tag;
				CollisionSensor& collisionSensorA = world->getComponent<CollisionSensor>(entityA);
				collisionSensorA.enteredEntities.push_back({entityB, {-normalVec.x, -normalVec.y}, tag, fixtureA->GetFilterData().categoryBits, fixtureA->GetFilterData().maskBits});
				collisionSensorA.stayingEntities.push_back({entityB, {-normalVec.x, -normalVec.y}, tag, fixtureA->GetFilterData().categoryBits, fixtureA->GetFilterData().maskBits});
			}

			if(world->hasComponent<CollisionSensor>(entityB) && !fixtureA->IsSensor()){
				int tag = fixtureB->tag;
				CollisionSensor& collisionSensorB = world->getComponent<CollisionSensor>(entityB);
				collisionSensorB.enteredEntities.push_back({entityA, normalVec, tag, fixtureB->GetFilterData().categoryBits, fixtureB->GetFilterData().maskBits});
				collisionSensorB.stayingEntities.push_back({entityA, normalVec, tag, fixtureB->GetFilterData().categoryBits, fixtureB->GetFilterData().maskBits});
			}
		}
	}

	void EndContact(b2Contact* contact) override{
		auto* fixtureA = (contact->GetFixtureA());
		auto* fixtureB = (contact->GetFixtureB());
		Entity entityA = (Entity)fixtureA->GetBody()->GetUserData().pointer;
		Entity entityB = (Entity)fixtureB->GetBody()->GetUserData().pointer;

		b2Vec2 posA = fixtureA->GetBody()->GetTransform().p;
		b2Vec2 posB = fixtureB->GetBody()->GetTransform().p;
		b2Vec2 normal = posA - posB;
		Vec2 normalVec{normal.x, normal.y};

		if(world->hasComponent<CollisionSensor>(entityA) && !fixtureB->IsSensor()){
			CollisionSensor& collisionSensorA = world->getComponent<CollisionSensor>(entityA);
			int tag = fixtureA->tag;

			for(int i=0; i<collisionSensorA.stayingEntities.size(); i++){
				if(collisionSensorA.stayingEntities[i].entity == entityB){
					collisionSensorA.stayingEntities.erase(collisionSensorA.stayingEntities.begin() + i);
					break;
				}
			}
			collisionSensorA.leavingEntities.push_back({entityB, {-normalVec.x, -normalVec.y}, tag, fixtureA->GetFilterData().categoryBits, fixtureA->GetFilterData().maskBits});
		}

		if(world->hasComponent<CollisionSensor>(entityB) && !fixtureA->IsSensor()){
			CollisionSensor& collisionSensorB = world->getComponent<CollisionSensor>(entityB);
			int tag = fixtureB->tag;
			for(int i=0; i<collisionSensorB.stayingEntities.size(); i++){
				if(collisionSensorB.stayingEntities[i].entity == entityA){
					collisionSensorB.stayingEntities.erase(collisionSensorB.stayingEntities.begin() + i);
					break;
				}
			}
			collisionSensorB.leavingEntities.push_back({entityA, normalVec, tag, fixtureB->GetFilterData().categoryBits, fixtureB->GetFilterData().maskBits});
		}
	}
};

std::string_view Physics::getName(){
	static std::string str{"Physics"};
	return str;
}

ISystem* Physics::clone(){
	Physics* physics = new Physics();
	physics->bodyTypeMap = this->bodyTypeMap;
	return physics;
}

template<class> inline constexpr bool always_false_v = false;

void Physics::createBody(World* world, Entity entity, b2WorldDS* b2WorldDS){
	auto b2World = b2WorldDS->world;
	auto worldScale = b2WorldDS->worldScale;
	auto tr = world->reduceComponent<Transform>(entity);
	auto& rigid = world->getComponent<RigidBody2D>(entity);

	b2BodyDef bodyDef;
	bodyDef.type = bodyTypeMap[rigid.bodyType];
	bodyDef.position.Set((tr.position.x+rigid.position.x)*worldScale, (tr.position.y+rigid.position.y)*worldScale);
	bodyDef.angle = tr.rotation + rigid.rotation;
	bodyDef.fixedRotation = rigid.fixedRotation;
	b2Body* body = b2World->CreateBody(&bodyDef);

	for(auto& fixture: rigid.fixtureVector){
		b2FixtureDef fixtureDef;
		fixtureDef.isSensor = fixture.isSensor;
		fixtureDef.density = fixture.density;
		fixtureDef.friction = fixture.friction;
		fixtureDef.restitution = fixture.restitution;
		fixtureDef.filter.categoryBits = fixture.collisionGroup;
		fixtureDef.filter.maskBits = fixture.collisionMask;

		b2CircleShape circleShape;
		b2PolygonShape polygonShape;
		b2ChainShape chainShape;

		std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, PointShape>){
				circleShape.m_radius = 0.1;
				fixtureDef.shape = &circleShape;
			}else if constexpr (std::is_same_v<T, RectangularShape>){
				polygonShape.SetAsBox(arg.width * tr.scale.x * worldScale/2., arg.height * tr.scale.y * worldScale/2., {fixture.position.x*worldScale, fixture.position.y*worldScale}, arg.rotation);
				fixtureDef.shape = &polygonShape;
			}else if constexpr (std::is_same_v<T, CircularShape>){
				circleShape.m_radius = arg.radio * ((tr.scale.x+tr.scale.y)/2.) * worldScale / 2.;
				fixtureDef.shape = &circleShape;
			}else if constexpr (std::is_same_v<T, RingShape>){
				circleShape.m_radius = arg.outerRadio * ((tr.scale.x+tr.scale.y)/2.) * worldScale;
				fixtureDef.shape = &circleShape;
			}else if constexpr (std::is_same_v<T, ChainShape>){
				//circleShape.m_radius = arg.outerRadio * ((tr.scale.x+tr.scale.y)/2.) * worldScale;
				std::vector<b2Vec2> vec{};
				for(Vec2 v: arg.vertices){
					b2Vec2 b2V{};
					b2V.x = v.x * tr.scale.x + fixture.position.x*worldScale;
					b2V.y = v.y * tr.scale.y + fixture.position.y*worldScale;
					vec.emplace_back(b2V);
				}
				if(vec.size() >= 2){
					chainShape.CreateLoop(vec.data(), vec.size());
					fixtureDef.shape = &chainShape;
				}else{
					circleShape.m_radius = 0.1;
					fixtureDef.shape = &circleShape;
				}
			}else if constexpr (std::is_same_v<T, ConeShape>){
				std::array<b2Vec2, b2_maxPolygonVertices> vertexArray;
				vertexArray[0] = {fixture.position.x*worldScale, fixture.position.y*worldScale};
				float angleStep = arg.aperture/(b2_maxPolygonVertices-2);
				float initAngle = -arg.aperture/2.;
				for(int i=1; i<b2_maxPolygonVertices; i++){
					float beta = initAngle + (i-1) * angleStep + fixture.rotation;
					vertexArray[i].x = arg.radio*cos(beta) + 0.5*(tr.scale.x+tr.scale.y)*worldScale*fixture.position.x;
					vertexArray[i].y = arg.radio*sin(beta) + 0.5*(tr.scale.x+tr.scale.y)*worldScale*fixture.position.y;
				}
				polygonShape.Set(vertexArray.data(), b2_maxPolygonVertices);
				fixtureDef.shape = &polygonShape;

			}else{ 
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
			}
		}, fixture.shape);

		body->CreateFixture(&fixtureDef);
	}

	body->GetUserData().pointer = entity;
	rigid.b2Body = body;
	rigid.updateBody = false;
}

void Physics::destroyBody(World* world, b2WorldDS* b2WorldDS, Entity entity){
	auto b2World = b2WorldDS->world;
	auto& rigid = world->getComponent<RigidBody2D>(entity);
	b2World->DestroyBody(rigid.b2Body);
}


void Physics::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { 
	if(!worldContext->b2World.createdWorld){
		resetb2World(world, &worldContext->b2World);
	}
	bodyTypeMap[BodyType::Static] = b2_staticBody;
	bodyTypeMap[BodyType::Kinematic] = b2_kinematicBody;
	bodyTypeMap[BodyType::Dynamic] = b2_dynamicBody;
}

void Physics::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	if(!worldContext->b2World.createdWorld){
		resetb2World(world, &worldContext->b2World);
	}else{
		createBody(world, entity, &worldContext->b2World);
	}
}

void Physics::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	if(worldContext->b2World.createdWorld){
		destroyBody(world, &worldContext->b2World, entity);
	}
}

void Physics::resetb2World(World* world, b2WorldDS* b2world){
	if(!b2world->createdWorld){
		CollisionListener* listener = new CollisionListener();
		CustomContactFilter* filter = new CustomContactFilter();
		listener->world = world;
		b2world->world = new b2World(b2world->gravity);
		b2world->world->SetContactFilter(filter);
		b2world->world->SetContactListener(listener);
		b2world->createdWorld = true;
	}
	for(auto& entity: this->mEntities){
		createBody(world, entity, b2world);
	}
}

void Physics::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto worldScale = worldContext->b2World.worldScale;
	static std::array<Transform, MAX_ENTITIES> transformBuffer;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	if(!worldContext->b2World.createdWorld){
		resetb2World(world, &worldContext->b2World);
	}

	if(!serviceContext->editorDS->isPlaying){ 
		for(auto& entity: this->mEntities){
			auto tr = world->reduceComponent<Transform>(entity);
			auto& rigid = world->getComponent<RigidBody2D>(entity);
			auto body = rigid.b2Body;
			if(rigid.updateBody){
				destroyBody(world, &worldContext->b2World, entity);
				createBody(world, entity, &worldContext->b2World);
			}else{
				body->SetTransform({(rigid.position.x+tr.position.x)*worldScale, (rigid.position.y+tr.position.y)*worldScale},tr.rotation+rigid.rotation); 
			}
		}
		return;
	}

	auto b2World = worldContext->b2World.world;
	//TODO set here the position/velocity of every body?
	for(auto& entity: this->mEntities){
		auto tr = world->reduceComponent<Transform>(entity);
		transformBuffer[entity] = tr;
		auto& rigid = world->getComponent<RigidBody2D>(entity);
		auto body = rigid.b2Body;

		b2Fixture* b2Fixture = body->GetFixtureList();             
		int i=0;
		int fixtureCount = rigid.fixtureVector.size() - 1;
		while(b2Fixture != NULL) {
			auto& fixture = rigid.fixtureVector[fixtureCount-i];
			b2Fixture->enable = fixture.enable;
			b2Fixture->tag = fixture.tag;
			b2Fixture = b2Fixture->GetNext();
			i += 1;
		}	
		if(rigid.bodyType == Kinematic || rigid.bodyType == Dynamic){
			body->SetAwake(true);
		}
		if(world->hasComponent<CollisionSensor>(entity)){
			CollisionSensor& collisionSensor = world->getComponent<CollisionSensor>(entity);
			collisionSensor.enteredEntities.clear();
			collisionSensor.leavingEntities.clear();
		}

		if(world->hasComponent<MovementVector>(entity)){
			auto& movement = world->getComponent<MovementVector>(entity);
			if(movement.enable){
				std::visit([&](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, MovementVelocityVector>){
						body->SetLinearVelocity({arg.vector.x*worldScale, arg.vector.y*worldScale});
						body->ApplyForceToCenter({0., 0.}, true);
					}else if constexpr (std::is_same_v<T, MovementForceVector>){
						body->ApplyForceToCenter({arg.vector.x*worldScale, arg.vector.y*worldScale}, true);
					}else{ 
						static_assert(always_false_v<T>, "non-exhaustive visitor!");
					}
				}, movement.movementVector);
			}
			else{
				body->SetLinearVelocity({0., 0.});
			}
		}

		if(rigid.updateBody){
			destroyBody(world, &worldContext->b2World, entity);
			createBody(world, entity, &worldContext->b2World);
		}else{
			body->SetTransform({(rigid.position.x+tr.position.x)*worldScale, (rigid.position.y+tr.position.y)*worldScale},tr.rotation+rigid.rotation); 
		}
	}
	b2World->Step(0.000001*Time::deltaTime, velocityIterations, positionIterations);
	for(auto& entity: this->mEntities){
		auto& tr = world->getComponent<Transform>(entity);
		auto& rigid= world->getComponent<RigidBody2D>(entity);
		auto body = rigid.b2Body;
		auto vec =  body->GetPosition();
		Transform newTr = transformBuffer[entity];
		
		newTr.rotation = body->GetAngle() - rigid.rotation;
		newTr.position.x = vec.x/worldScale - rigid.position.x;
		newTr.position.y = vec.y/worldScale - rigid.position.y;
		tr += newTr - transformBuffer[entity];
	}
}

Physics::~Physics(){ }

