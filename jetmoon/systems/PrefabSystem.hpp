#pragma once
#include <string_view>           // for string_view
#include "core/ISystem.hpp"      // for ISystem
#include "core/definitions.hpp"  // for Entity
class World;
struct ServiceContext;
struct WorldContext;

class PrefabSystem: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;
		~PrefabSystem();
};
