#pragma once
#include "core/ISystem.hpp"
class World;
struct ServiceContext;
struct WorldContext;

class SaveEntityMapSystem: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;
		~SaveEntityMapSystem();
};
