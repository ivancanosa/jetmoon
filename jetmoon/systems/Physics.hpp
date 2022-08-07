#pragma once
#include <array>                 // for array
#include <string_view>           // for string_view
#include "box2d/b2_body.h"       // for b2BodyType
#include "core/ISystem.hpp"      // for ISystem
#include "core/definitions.hpp"  // for Entity
class World;
struct ServiceContext;
struct WorldContext;
struct b2WorldDS;



class Physics: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 

		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void resetb2World(World* world, b2WorldDS* b2world);

		std::string_view getName() override;

		ISystem* clone() override;

		~Physics();

	private:
		void createBody(World* world, Entity entity, b2WorldDS* b2WorldDS);
		void destroyBody(World* world, b2WorldDS* b2WorldDS, Entity entity);

		std::array<b2BodyType, 3> bodyTypeMap{};
};

