#pragma once
#include <memory>
#include <stdint.h>                     // for int32_t
#include <array>                        // for array
#include <deque>                        // for deque
#include <string_view>                  // for string_view
#include "core/ISystem.hpp"             // for ISystem
#include "core/definitions.hpp"         // for Entity, MAX_ENTITIES, Vec3
#include "glm/detail/type_vec2.hpp"     // for vec
#include "glm/detail/type_vec3.hpp"     // for vec
#include "glm/detail/type_vec4.hpp"     // for vec
#include "glm/ext/matrix_float4x4.hpp"  // for mat4
#include "glm/ext/vector_float2.hpp"    // for vec2
#include "glm/ext/vector_float3.hpp"    // for vec3
#include "glm/ext/vector_float4.hpp"    // for vec4
#include "opengl/BatchRenderer.hpp"     // for BatchRenderer (ptr only), LINE
class World;
struct ServiceContext;
struct WorldContext;

struct TrailVertexGL{
	glm::vec3 position;
	glm::vec2 uv;
	float textureIndex;
	glm::vec2 adjacentPoint;
	glm::vec4 color;
	float width;
};

struct TrailVertexData{
	Vec3 position;
	glm::mat4 tr;
	int32_t time; // in microseconds
	int32_t maxTime;
};

class TrailSystem: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;
		~TrailSystem();

	private:
		std::array<std::deque<TrailVertexData>, MAX_ENTITIES> trailQueueArray{};
		std::shared_ptr<BatchRenderer<TrailVertexGL, 1024, RenderElement::LINE>> batchRendererTrails;
};
