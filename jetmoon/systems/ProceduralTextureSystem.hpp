#pragma once
#include <memory>
#include <array>                  // for array
#include <string_view>            // for string_view
#include "GL/glew.h"              // for GLuint
#include "core/ISystem.hpp"       // for ISystem
#include "core/definitions.hpp"   // for Entity, MAX_ENTITIES
class Framebuffer;
class World;
struct ServiceContext;
struct WorldContext;

class ProceduralTextureSystem: public ISystem{
	public:
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;

		~ProceduralTextureSystem();
	private:
		std::array<std::shared_ptr<Framebuffer>, MAX_ENTITIES> framebuffersArray;
		GLuint VAO, VBO, EBO;
};
