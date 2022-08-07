#include "TilemapSystem.hpp"
#include <assert.h>                         // for assert
#include <stddef.h>                         // for offsetof
#include <tuple>
#include <filesystem>                       // for path
#include <iosfwd>                           // for string
#include <set>                              // for set
#include <string>                           // for allocator, operator==
#include <type_traits>                      // for remove_extent_t
#include "commonStructures/Camera.hpp"      // for Camera
#include "components/RigidBody2D.hpp"       // for Fixture, RigidBody2D
#include "components/TilemapComponent.hpp"  // for TilemapComponent
#include "components/Transform.hpp"         // for Transform
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "core/ServiceContext.hpp"          // for ServiceContext, EditorDS
#include "core/World.hpp"                   // for World
#include "core/WorldContext.hpp"            // for WorldContext
#include "opengl/Framebuffer.hpp"           // for FramebufferDefinition
#include "opengl/Shader.hpp"                // for Shader
#include "services/ImageLoader.hpp"         // for ImageLoader
#include "services/Renderer2D.hpp"          // for Renderer2D
#include "services/ShaderLoader.hpp"        // for ShaderLoader
#include "services/EditorDS.hpp"        
#include "utils/Shapes.hpp"                 // for ChainShape, ShapeVariant
#include "utils/marchingSquares.hpp"        // for marchingSquares
#include "utils/tileson.hpp"                // for Vector2, Map, Rect, Layer
namespace mapbox { namespace util { template <std::size_t I, typename T> struct nth; } }

namespace mapbox {
namespace util {

template <>
struct nth<0, Vec2> {
    inline static auto get(const Vec2 &t) {
        return t.x;
    };
};
template <>
struct nth<1, Vec2> {
    inline static auto get(const Vec2 &t) {
        return t.y;
    };
};

} // namespace util
} // namespace mapbox

void TilemapSystem::initTilemapData(int tilesCount){
	tilemapData[actualEntity] = std::make_shared<TilemapData>();

	GLuint vao, vbo, ibo;

	glCreateBuffers(1, &vbo);	
	glNamedBufferStorage(vbo, sizeof(TileVertex)*tilesCount * 4, nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ibo);
	GLuint* trianglesIndex = new GLuint[tilesCount * 6];
	for(int i=0; i < tilesCount ; i++) {
		trianglesIndex[i*6 + 0] = 0 + i*4;
		trianglesIndex[i*6 + 1] = 1 + i*4;
		trianglesIndex[i*6 + 2] = 3 + i*4;
		trianglesIndex[i*6 + 3] = 1 + i*4;
		trianglesIndex[i*6 + 4] = 2 + i*4;
		trianglesIndex[i*6 + 5] = 3 + i*4;
	}
	glNamedBufferStorage(ibo, sizeof(GLuint)*tilesCount*6, trianglesIndex, GL_DYNAMIC_STORAGE_BIT);
	delete[] trianglesIndex;

	glCreateVertexArrays(1, &vao);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(TileVertex));
	glVertexArrayElementBuffer(vao, ibo);

	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glEnableVertexArrayAttrib(vao, 2);
	glEnableVertexArrayAttrib(vao, 3);
	glEnableVertexArrayAttrib(vao, 4);

	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TileVertex, position));
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(TileVertex, texCoord));
	glVertexArrayAttribFormat(vao, 2, 1, GL_FLOAT, GL_FALSE, offsetof(TileVertex, textureIndex));
	glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, offsetof(TileVertex, textureClip));
	glVertexArrayAttribFormat(vao, 4, 1, GL_INT, GL_FALSE, offsetof(TileVertex, entity));

	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribBinding(vao, 2, 0);
	glVertexArrayAttribBinding(vao, 3, 0);
	glVertexArrayAttribBinding(vao, 4, 0);
	tilemapData[actualEntity]->VAO = vao;
	tilemapData[actualEntity]->VBO = vbo;
	tilemapData[actualEntity]->IBO = ibo;
	tilemapData[actualEntity]->tileVertexBuffer.reserve(tilesCount * 4);
}

void TilemapSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	auto& tilemapComponent = world->getComponent<TilemapComponent>(entity);
	tilemapData[entity] = nullptr;
	actualEntity = entity;
	tson::Tileson t;
	map = t.parse(fs::path(tilemapComponent.tilemap));
	if(map->getStatus() != tson::ParseStatus::OK) return;
	auto tileSize {map->getTileSize()};
	auto mapSize {map->getSize()};
	this->tileSize = {tileSize.x, tileSize.y};
	this->matrixSize = {mapSize.x, mapSize.y};
	this->worldSize = {tileSize.x * mapSize.x, tileSize.y * mapSize.y};
	tilemapComponent.collisionMatrix.resize(this->matrixSize[0]*this->matrixSize[1], false);
	std::fill(tilemapComponent.collisionMatrix.begin(), tilemapComponent.collisionMatrix.end(), false);
	auto& tr = world->getComponent<Transform>(entity);
	tr.scale.x = mapSize.x;
	tr.scale.y = mapSize.y;

	tilemapComponent.matrixSizeX = mapSize.x;
	tilemapComponent.matrixSizeY = mapSize.y;

	int xChunkCount = mapSize.x/chunkSize;
	int yChunkCount = mapSize.y/chunkSize;
	assert(chunkSize <= mapSize.x && chunkSize <= mapSize.y && "The chunk size must be less or equal to the map tiles size");
	assert(mapSize.x % chunkSize == 0 && mapSize.y % chunkSize == 0 && "The map size must be divisible by the chunk size");

	FramebufferDefinition fbDef { (GLuint)chunkSize * tileSize.x, (GLuint)chunkSize * tileSize.y, false, {
		{GL_RGBA, GL_RGBA}
	}};

	for(int y=0; y<yChunkCount; y++){
		for(int x=0; x<xChunkCount; x++){
			tilesToProcess.insert( {{x,y}, {}} );
		}
	}

	//TODO: Maybe this causes the actual frame to look bad
	int tilesCount = 0;
	for(auto& layer: map->getLayers()){
		if(layer.getType() == tson::LayerType::ObjectGroup){
			//			parseObjectLayer(&layer, world);
		}else if (layer.getType() == tson::LayerType::TileLayer){
			if(!layer.isVisible()) continue;
			for(auto&[id, tile]: layer.getTileObjects()){
				tilesCount += 1;
				auto[x, y] = id;
				int xChunk = x/chunkSize;
				int yChunk = y/chunkSize;
				tilesToProcess[{xChunk, yChunk}].push_back( {x, y, tile} );
			}

		}else{
			assert(false && "Layer from Tilemap not recognized");
		}
	}

	initTilemapData(tilesCount);
	for(auto&[index, _]: tilesToProcess){
		auto [x, y] = index;
		renderChunk(x, y, entity, world, serviceContext);
	}
	tilesToProcess.clear();
	glBindVertexArray(tilemapData[actualEntity]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tilemapData[actualEntity]->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tilemapData[actualEntity]->tileVertexBuffer.size() * sizeof(TileVertex), tilemapData[actualEntity]->tileVertexBuffer.data()); 

	// Generation of collision edges
	if(world->hasComponent<RigidBody2D>(entity)){
		world->removeComponent<RigidBody2D>(entity);
	}
	RigidBody2D rigid{};
	rigid.fixtureVector.clear();

	auto solution = marchingSquares(tilemapComponent.collisionMatrix, this->matrixSize[0], this->matrixSize[1], {1., 1.});
	for(auto& v: solution){
		Fixture fixture{};
		ChainShape chainShape{};
		std::reverse(std::get<1>(v).begin(), std::get<1>(v).end());
		for(auto& vertex: std::get<1>(v)){
			vertex.x /= this->matrixSize[0];
			vertex.y /= this->matrixSize[1];
			vertex.x -= 0.5;
			vertex.y -= 0.5;
			chainShape.vertices.emplace_back(vertex);
		}
		fixture.shape = chainShape;
		fixture.collisionGroup = tilemapComponent.collisionGroup;
		fixture.collisionMask = tilemapComponent.collisionMask;
		rigid.fixtureVector.emplace_back(fixture);
		tilemapComponent.isFixtureHole.emplace_back(std::get<0>(v));
	}

	rigid.updateBody = true;
	tilemapComponent.generated = true;
	world->addComponent<RigidBody2D>(entity, rigid);
	tilemapComponent.regenerate = false;
}

void TilemapSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void TilemapSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	bool reload = serviceContext->editorDS->reload;
	if(reload){
		tilesToProcess.clear();
	}
	for(auto entity: mEntities){
		auto tilemapComponent = world->getComponent<TilemapComponent>(entity);
		if(reload) tilemapComponent.generated = false;
		if(tilemapComponent.generated == false || tilemapComponent.regenerate){
			addedEntity(entity, world, worldContext, serviceContext);
		}else{
			auto tr = world->reduceTransformMatrix(entity);
			auto shader = serviceContext->shaderLoader->getShader("tilemapShader");
			shader.bind();
			auto& camera = worldContext->camera;
			shader.setUniform("viewTransform", camera.getTransformationMatrix() * tr);
			glBindVertexArray(tilemapData[entity]->VAO);
			for(int i=0; i < tilemapData[entity]->texturesId.size(); i++){
				glBindTextureUnit(i, tilemapData[entity]->texturesId[i]);
			}
			glDrawElements(GL_TRIANGLES, 6 * (tilemapData[entity]->tileVertexBuffer.size()/4), GL_UNSIGNED_INT, 0);
			//TODO Statistics here
		}
	}
	serviceContext->renderer2D->flushAll();
}

void TilemapSystem::renderChunk(int xChunkIndex, int yChunkIndex, Entity entity, World* world, ServiceContext* serviceContext){
	auto& tilemapComponent = world->getComponent<TilemapComponent>(entity);
	auto loader = serviceContext->imageLoader.get();
	glm::vec4 originRect{0., 0., 1., 1.};

	static constexpr glm::vec4 originVertexPos[4] = {
		{ 0.5,  0.5, 0., 1.},
		{ 0.5, -0.5, 0., 1.},
		{-0.5, -0.5, 0., 1.},
		{-0.5,  0.5, 0., 1.}
	};
	static constexpr glm::vec2 originTextureCoord[4] = {
		{1.0, 1.0 },
		{1.0, 0.0 },
		{0.0, 0.0 },
		{0.0, 1.0 }
	};

	for(auto&[x, y, tile]: tilesToProcess[{xChunkIndex, yChunkIndex}]){
		tson::Tileset* tileset = tile.getTile()->getTileset();
		tson::Rect drawingRect = tile.getDrawingRect(); //this is the rect to draw from the tileset
		auto tilesetSize = tileset->getImageSize();
		auto tilesetID = tileset->getImage().stem().string();
		auto collision = tile.getTile()->getType();

		assert((collision == "collision" || collision == "") && "Tile Type not recognized");

		originRect[0] = float(drawingRect.x)/float(tilesetSize.x);
		originRect[1] = 1. - float(drawingRect.y + tileSize[1])/float(tilesetSize.y);
		originRect[2] = float(drawingRect.width)/float(tilesetSize.x);
		originRect[3] = float(drawingRect.height)/float(tilesetSize.y);

		bool prev = tilemapComponent.collisionMatrix[x+this->matrixSize[0]*(this->matrixSize[1] - y - 1)];
		tilemapComponent.collisionMatrix[x+this->matrixSize[0]*(this->matrixSize[1] - y - 1)] = prev || (collision == "collision");

		x %= chunkSize;
		y %= chunkSize;

		auto scale = 2./chunkSize;
		y = chunkSize - y - 1;
		auto trans = glm::mat4(1.0f);
		float tileWidth = 1. * scale;
		float shift = -tileWidth*((2/scale)/2 - 1) - tileWidth/2;

		trans = glm::scale(trans, glm::vec3(scale/2., scale/2., 1.f));
		trans = glm::translate(trans, glm::vec3((shift + tileWidth*x)/scale, (shift + tileWidth*y)/scale, 0.f));


		TileVertex tileVertex;
		auto id = loader->load(tilesetID);
		float textureId{0};
		auto it = std::find(tilemapData[actualEntity]->texturesId.begin(), tilemapData[actualEntity]->texturesId.end(), id);
		if(it == tilemapData[actualEntity]->texturesId.end()){
			tilemapData[actualEntity]->texturesId.push_back(id);
			textureId = tilemapData[actualEntity]->texturesId.size() - 1;
		}else{
			textureId = it - tilemapData[actualEntity]->texturesId.begin();
		}
		assert(textureId < 32);
		for(int i=0; i<4; i++){
			tileVertex.position = trans * originVertexPos[i];
			tileVertex.texCoord = originTextureCoord[i];
			tileVertex.textureIndex = textureId;
			tileVertex.textureClip = originRect;
			tileVertex.entity = actualEntity;
			tilemapData[actualEntity]->tileVertexBuffer.push_back(tileVertex);
		}
		
	}
}

void TilemapSystem::parseObjectLayer(tson::Layer* layer, World* world){
/*	auto factory = Locator<EntityFactory>::getService();
	for(auto& obj: layer->getObjects()){
		auto pos = obj.getPosition();
		auto entity = factory->loadPrefab(obj.getType(), world);
		auto& transform = world->getComponent<Transform>(entity);
		std::cout << transform.position.x << std::endl;
		transform.position.x = float(pos.x)/float(tileSize[0]);
		transform.position.y = float(worldSize[1] - pos.y )/float(tileSize[1]);
	}*/
}

std::string_view TilemapSystem::getName(){
	static std::string str{"TilemapSystem"};
	return str;
}

ISystem* TilemapSystem::clone(){ 
	TilemapSystem* tilemap = new TilemapSystem();
	tilemap->tilemapData = tilemapData;
	return tilemap;
}
