#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdelete-abstract-non-virtual-dtor"
#pragma clang diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#include <memory>
#include <stdint.h>                   // for int32_t
#include <array>                      // for array
#include <map>                        // for map
#include <string_view>                // for string_view
#include <tuple>                      // for tuple
#include <vector>                     // for vector
#include "GL/glew.h"                  // for GLuint, glDeleteVertexArrays
#include "core/ISystem.hpp"           // for ISystem
#include "core/definitions.hpp"       // for Entity, NullEntity, MAX_ENTITIES
#include "glm/ext/vector_float2.hpp"  // for vec2
#include "glm/ext/vector_float3.hpp"  // for vec3
#include "glm/ext/vector_float4.hpp"  // for vec4
#include "utils/tileson.hpp"          // for Map, Layer (ptr only), TileObje...
class World;
struct ServiceContext;
struct WorldContext;

struct TileVertex{
	glm::vec3 position;
	glm::vec2 texCoord;
	float textureIndex;
	glm::vec4 textureClip;
	int32_t entity;
};

struct TilemapData{
	GLuint VAO{0}, VBO, IBO;
	std::vector<TileVertex> tileVertexBuffer{};
	std::vector<GLuint> texturesId{};

	~TilemapData(){
		glDeleteVertexArrays(1, &VAO);
	};
};

class TilemapSystem: public ISystem{
public:
	void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	std::string_view getName() override;
	ISystem* clone() override;
private:
	void parseObjectLayer(tson::Layer* layer, World* world);
	void renderChunk(int xChunkIndex, int yChunkIndex, Entity entity, World* world, ServiceContext* serviceContext);

	void initTilemapData(int tilesCount);

	std::array<int, 2> tileSize{};
	std::array<int, 2> matrixSize{};
	std::array<int, 2> worldSize{};
	std::unique_ptr<tson::Map> map;


	std::map<std::tuple<int, int>, std::vector<std::tuple<int, int, tson::TileObject>>> tilesToProcess;

	const int chunkSize = 32; 

	std::array<std::shared_ptr<TilemapData>, MAX_ENTITIES> tilemapData{nullptr};
	Entity actualEntity{NullEntity};
};

#pragma clang diagnostic pop
