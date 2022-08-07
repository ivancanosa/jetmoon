#pragma once

#include <set>
#include <string_view>
#include "definitions.hpp"

class World;
struct ServiceContext;
struct WorldContext;


class ISystem {
public:
	std::set<Entity> mEntities; //TODO: This data type is not very efficient
	virtual void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){}
	virtual void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){}
	virtual void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) = 0;
	virtual void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) = 0;
	virtual std::string_view getName(){static std::string str{"defaultSystemName"}; return str;}
	virtual ISystem* clone() = 0;
	virtual ~ISystem() = default;
};
