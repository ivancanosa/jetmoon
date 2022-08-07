#pragma once

#include <iosfwd>                // for string
#include "core/definitions.hpp"  // for Entity, NullEntity
class World;


class SceneSerializer{
public:
	void saveWorld(World* world, std::string path="savedWorld.scene");
	void savePrefab(World* world, Entity entity, std::string path);
	Entity loadPrefab(World* world, std::string prefabPath, Entity entity = NullEntity);
	void loadWorld(World* world, std::string scenePath);
	std::string serializeEntityToString(Entity entity, World* world);
	std::string serializeWorld(World* world);

	Entity duplicateEntity(World* world, Entity entity);
};

