#include "systems/PrefabSystem.hpp"
#include <iosfwd>                          // for string
#include "components/PrefabComponent.hpp"  // for PrefabComponent
#include "core/SceneSerializer.hpp"        // for SceneSerializer
#include "core/World.hpp"                  // for World
struct ServiceContext;
struct WorldContext;


std::string_view PrefabSystem::getName(){
	static std::string str{"PrefabSystem"};
	return str;
}

ISystem* PrefabSystem::clone(){
	return new PrefabSystem();
}

void PrefabSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void PrefabSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	SceneSerializer serializer;
	auto& prefabComponent = world->getComponent<PrefabComponent>(entity);
	if(prefabComponent.load){
		serializer.loadPrefab(world, prefabComponent.prefab, entity);
	}
}

void PrefabSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void PrefabSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

PrefabSystem::~PrefabSystem(){ }

