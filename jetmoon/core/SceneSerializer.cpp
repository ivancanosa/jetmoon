#include "core/SceneSerializer.hpp"
#include <assert.h>                          // for assert
#include <chrono>                            // for filesystem
#include <cstdio>                            // for remove, size_t
#include <filesystem>                        // for exists, path
#include <fstream>                           // for string, char_traits, ifs...
#include <initializer_list>                  // for initializer_list
#include <set>                               // for set
#include <string>                            // for operator==, hash
#include <string_view>                       // for operator==, operator!=
#include <unordered_set>                     // for unordered_set, unordered...
#include <utils/type_name.hpp>               // for type_name
#include <utils/variant_by_index.hpp>        // for variant_by_index
#include <variant>                           // for visit, variant, variant_...
#include <vector>                            // for vector
#include "ComponentVariant.hpp"              // for ComponentVariant
#include "components/Hierarchy.hpp"          // for Hierarchy
#include "components/NameComponent.hpp"      // for NameComponent
#include "components/PrefabComponent.hpp"    // for PrefabComponent
#include "components/Transform.hpp"          // for Transform
#include "core/World.hpp"                    // for World
#include "core/WorldContext.hpp"             // for WorldContext
#include "core/definitions.hpp"              // for Entity, createUUID, Null...
#include "utils/json.hpp"                    // for basic_json, json, basic_...

struct SerializedEntity{
	UUID uuid;
	std::vector<ComponentVariant> components{};
	std::vector<SerializedEntity> children{};

	void regenerateUUID(){
		uuid = createUUID();
		for(auto& child: children){
			child.regenerateUUID();
		}
	}

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SerializedEntity, uuid, components, children);


struct SerializedScene{
	std::vector<SerializedEntity> entities{};
	WorldContext worldContext{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SerializedScene, entities, worldContext);

Entity loadSerializedEntity(World* world, SerializedEntity& serializedEntity, Entity entity, bool isPrefab=false){
	if(!world->hasComponent<Hierarchy>(entity)){
		world->addComponent<Hierarchy>(entity, {});
	}
	bool hasPrefab{false};
	std::unordered_set<std::string>* changedComponents{nullptr};
	if(world->hasComponent<PrefabComponent>(entity)){
		hasPrefab = true;
		auto& component = world->getComponent<PrefabComponent>(entity);
		changedComponents = &component.changedComponents;
	}
	for(auto& component: serializedEntity.components){
		std::visit([=](auto arg){
				if constexpr(type_name<decltype(arg)>() == "PrefabComponent"){
				arg.load = isPrefab;
				}
				if(hasPrefab){
				if(changedComponents->find(std::string(type_name<decltype(arg)>())) == changedComponents->end() &&
						!world->hasComponent<decltype(arg)>(entity)){
				world->addComponent<decltype(arg)>(entity, arg);
				}
				}else{
				if(!world->hasComponent<decltype(arg)>(entity)){
				world->addComponent<decltype(arg)>(entity, arg);
				}
				}
				}, component);
	}
	for(SerializedEntity& serializedChild: serializedEntity.children){
		Entity child = world->createEntityInternal(serializedEntity.uuid);
		loadSerializedEntity(world, serializedChild, child);
		world->addChild(entity, child);
	}
	return entity;
}

SerializedEntity serializeEntity(Entity entity, World* world, bool saveFull=false){
	SerializedEntity serialized{};
	serialized.uuid = world->getUUID(entity);

	if(!world->hasComponent<PrefabComponent>(entity) || saveFull){
		variant_by_index<ComponentVariant> type_indexer;
		std::size_t variantCount = std::variant_size_v<ComponentVariant>;
		for (std::size_t i=0; i < variantCount; i++){
			std::visit([&](auto arg){
					if(world->hasComponent<decltype(arg)>(entity)){
					if constexpr (type_name<decltype(arg)>() != "Hierarchy"){
					serialized.components.push_back(world->getComponent<decltype(arg)>(entity));
					}
					}
					}, type_indexer.make_default(i));
		}

		if(world->hasComponent<Hierarchy>(entity)){
			Hierarchy& h = world->getComponent<Hierarchy>(entity);
			for(auto childEntity: h.children){
				serialized.children.push_back(serializeEntity(childEntity, world));
			}
		}
	}else{
		auto* changedComponents = &(world->getComponent<PrefabComponent>(entity).changedComponents);
		variant_by_index<ComponentVariant> type_indexer;
		std::size_t variantCount = std::variant_size_v<ComponentVariant>;
		for (std::size_t i=0; i < variantCount; i++){
			std::visit([&](auto arg){
					if(world->hasComponent<decltype(arg)>(entity)){
					if constexpr (type_name<decltype(arg)>() == "Hierarchy") return;
					if constexpr (type_name<decltype(arg)>() == "NameComponent") return;
					if constexpr (type_name<decltype(arg)>() == "Transform") return;
					if constexpr (type_name<decltype(arg)>() == "PrefabComponent") return;
					if(changedComponents->find(std::string(type_name<decltype(arg)>())) != changedComponents->end()){
					serialized.components.push_back(world->getComponent<decltype(arg)>(entity));
					}
					}
					}, type_indexer.make_default(i));
		}

		auto& tr = world->getComponent<Transform>(entity);
		auto& prefab = world->getComponent<PrefabComponent>(entity);
		if(world->hasComponent<NameComponent>(entity)){
			serialized.components.push_back(world->getComponent<NameComponent>(entity));
		}
		serialized.components.push_back(tr);
		serialized.components.push_back(prefab);
	}
	return serialized;
}



void SceneSerializer::saveWorld(World* world, std::string path){
	std::string data = serializeWorld(world);

	if(std::filesystem::exists(path)) std::remove(path.c_str());
	std::ofstream out(path);
	out << data;
	out.close();
}

void SceneSerializer::savePrefab(World* world, Entity entity, std::string path){
	auto serializedEntity = serializeEntityToString(entity, world);

	if(std::filesystem::exists(path)) std::remove(path.c_str());
	std::ofstream out(path);
	out << serializedEntity;
	out.close();
}

Entity SceneSerializer::loadPrefab(World* world, std::string prefabPath, Entity entity){
	namespace fs = std::filesystem;
	assert(std::filesystem::exists(prefabPath) && "The prefab path does not exists");
	fs::path file{prefabPath};
	std::ifstream t(file.string());
	std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	auto j = nlohmann::json::parse(jsonStr);
	auto prefab = j.get<SerializedEntity>();
	prefab.regenerateUUID();
	if(entity == NullEntity){
		entity = world->createEntityInternal(prefab.uuid);
	}
	loadSerializedEntity(world, prefab, entity);
	t.close();
	return entity;
}

void SceneSerializer::loadWorld(World* world, std::string scenePath){
	namespace fs = std::filesystem;
	if(!std::filesystem::exists(scenePath)) return;
	fs::path file{scenePath};
	std::ifstream t(file.string());
	std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	auto j = nlohmann::json::parse(jsonStr);
	auto scene = j.get<SerializedScene>();
	world->worldContext = std::make_shared<WorldContext>(scene.worldContext);
	for(auto& rootSerializedEntity: scene.entities){
		Entity entity = world->createEntityInternal(rootSerializedEntity.uuid);
		loadSerializedEntity(world, rootSerializedEntity, entity, true);
	}
	t.close();
}


std::string SceneSerializer::serializeEntityToString(Entity entity, World* world){
	nlohmann::json j(serializeEntity(entity, world, true));
	return j.dump(2);
}

std::string SceneSerializer::serializeWorld(World* world){
	SerializedScene scene{};
	scene.worldContext = WorldContext(*world->worldContext);
	for(auto& entity: world->getRootEntitiesSet()){
		scene.entities.push_back(serializeEntity(entity, world));
	}
	nlohmann::json j(scene);
	return j.dump(2);
}

Entity SceneSerializer::duplicateEntity(World* world, Entity entity){
	Entity duplicate = world->createEntity();
	variant_by_index<ComponentVariant> type_indexer;
	std::size_t variantCount = std::variant_size_v<ComponentVariant>;
	for (std::size_t i=0; i < variantCount; i++){
		std::visit([&](auto arg){
				if(world->hasComponent<decltype(arg)>(entity) && !world->hasComponent<decltype(arg)>(duplicate)){
				if constexpr (type_name<decltype(arg)>() != "Hierarchy" && type_name<decltype(arg)>() != "Transform"){
				world->addComponent<decltype(arg)>(duplicate, world->getComponent<decltype(arg)>(entity));
				}
				}
				}, type_indexer.make_default(i));
	}

	world->getComponent<Transform>(duplicate) = world->getComponent<Transform>(entity);

	if(world->hasComponent<Hierarchy>(entity)){
		Hierarchy& h = world->getComponent<Hierarchy>(entity);
		for(auto childEntity: h.children){
			Entity childEntityDup = duplicateEntity(world, childEntity);
			world->addChild(duplicate, childEntityDup);
		}
	}
	return duplicate;
}
