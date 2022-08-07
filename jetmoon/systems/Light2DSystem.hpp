#pragma once
#include <memory>
#include <string_view>                // for string_view
#include "GL/glew.h"                  // for GLuint
#include "core/ISystem.hpp"           // for ISystem
#include "core/definitions.hpp"       // for Entity
#include "glm/detail/type_vec2.hpp"   // for vec
#include "glm/detail/type_vec3.hpp"   // for vec
#include "glm/ext/vector_float2.hpp"  // for vec2
#include "glm/ext/vector_float3.hpp"  // for vec3
#include "opengl/BatchRenderer.hpp"   // for BatchRenderer (ptr only), Rende...
#include "services/Renderer2D.hpp"    // for BATCH_MAX_ELEMENTS
class Framebuffer;
class World;
struct ServiceContext;
struct WorldContext;

struct LightVertex{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
	float intensity;
};

class Light2DSystem: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;
		~Light2DSystem();
	private:
		std::shared_ptr<BatchRenderer<LightVertex, BATCH_MAX_ELEMENTS, RenderElement::SQUAD>> batchRendererLights;
		static bool initializedFb;
		static std::shared_ptr<Framebuffer> maskFb;
		static std::shared_ptr<Framebuffer> auxFb;
		static GLuint VAO, VBO, EBO;
};

