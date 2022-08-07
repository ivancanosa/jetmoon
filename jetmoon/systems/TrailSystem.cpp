#include "systems/TrailSystem.hpp"
#include <stddef.h>                               // for offsetof
#include <cmath>                                  // for atan, sqrt
#include <iosfwd>                                 // for string
#include <set>                                    // for set
#include <string>                                 // for allocator, char_traits
#include <type_traits>                            // for remove_extent_t
#include "GL/glew.h"                              // for GL_FLOAT, GLuint
#include "commonStructures/Camera.hpp"            // for Camera
#include "commonStructures/EngineStatistics.hpp"  // for EngineStatistics
#include "components/Trail.hpp"                   // for Trail
#include "components/Transform.hpp"               // for Transform, fromMatrix
#include "core/ServiceContext.hpp"                // for ServiceContext
#include "core/Time.hpp"                          // for Time, Time::deltaTime
#include "core/World.hpp"                         // for World
#include "core/WorldContext.hpp"                  // for WorldContext
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp> 
#include "services/ImageLoader.hpp"               // for ImageLoader
#include "services/Renderer2D.hpp"                // for Renderer2D
#include "services/ShaderLoader.hpp"              // for ShaderLoader
#include "core/interpolation.hpp"

void TrailSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	auto trailShader = serviceContext->shaderLoader->getShader("trailShader");
	batchRendererTrails = std::make_shared<BatchRenderer<TrailVertexGL, 1024, RenderElement::LINE>>();
	batchRendererTrails->init(trailShader, {
		{GL_FLOAT, 3, offsetof(TrailVertexGL, position)},
		{GL_FLOAT, 2, offsetof(TrailVertexGL, uv)},
		{GL_FLOAT, 1, offsetof(TrailVertexGL, textureIndex)},
		{GL_FLOAT, 2, offsetof(TrailVertexGL, adjacentPoint)},
		{GL_FLOAT, 4, offsetof(TrailVertexGL, color)},
		{GL_FLOAT, 1, offsetof(TrailVertexGL, width)},
	});
	batchRendererTrails->setGraphicsStatistics(&serviceContext->engineStatistics->graphicsStatistics);
}

void TrailSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto& trail = world->getComponent<Trail>(entity);
	auto tr = fromMatrix(world->reduceTransformMatrix(entity));
	trail.firstPoint = {tr.position.x, tr.position.y};
}

void TrailSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void TrailSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	batchRendererTrails->setViewTransform(worldContext->camera.getTransformationMatrix());
	for(auto&& entity: this->mEntities){
		auto& trail = world->getComponent<Trail>(entity);
		auto tr = fromMatrix(world->reduceTransformMatrix(entity));
		auto& queue = trailQueueArray[entity];
		bool newPoint = true;
		bool first{true};
		for(auto& vertex: queue){
			vertex.time -= Time::deltaTime;
			if(vertex.time <= 0 && first){
				trail.firstPoint = {vertex.position.x, vertex.position.y};
				queue.pop_front();
			}
			first=false;
		}

		if(queue.size() > 0){
			const auto& vertex = queue.back();
			float dist = std::sqrt((vertex.position.x-tr.position.x)*(vertex.position.x-tr.position.x) + (vertex.position.y-tr.position.y)*(vertex.position.y-tr.position.y));
			if(dist >= trail.minVertexDistance){
				newPoint = true;
			}else{
				newPoint = false;
			}
		}
		if(newPoint){
			auto time = int32_t(trail.time*1000000);
			if(!trail.renderAsSprites){
				queue.push_back({{tr.position.x, tr.position.y}, glm::mat4(1.0f), time, time});
			}else{
				Vec2 v0, v1;
				if(queue.size() > 0){
					auto& v = queue.back();
					v0 = {v.position.x, v.position.y};
				}else{
					v0 = trail.firstPoint;
				}

				v1 = {tr.position.x, tr.position.y};
				Vec2 dir = v1 - v0;
				float rotation = std::atan(dir.y/dir.x);
				auto trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((tr.position.x), (tr.position.y), tr.position.z));
				trans = glm::rotate(trans, rotation, glm::vec3(0.f, 0.f, 1.0f));
				queue.push_back({{tr.position.x, tr.position.y}, trans, time, time});
			}
		}

		float texture;
		//Render the trail
		if(trail.texture != ""){
			texture = serviceContext->imageLoader->load(trail.texture);
		}else{
			texture = serviceContext->imageLoader->load("white");
		}
		glm::vec2 adjacentPoint0={trail.firstPoint.x, trail.firstPoint.y}, adjacentPoint1;
		float accUv = 0.;
		if(!trail.renderAsSprites){
			for(int i = 1; i < queue.size(); i++){
				TrailVertexGL vertex[2];
				if(i >= queue.size()-1){
					adjacentPoint1 = {tr.position.x, tr.position.y};
				}else{
					auto& v0 = queue[i+1];
					adjacentPoint1 = {v0.position.x, v0.position.y};
				}
				auto& v0 = queue[i-1];
				auto& v1 = queue[i];
				float t0 = 1. - (float)v0.time/((float)v0.maxTime);
				float t1 = 1. - (float)v1.time/((float)v1.maxTime);
				auto color0 = interpolate(trail.color, t0);
				auto width0 = interpolate(trail.width, t0);
				auto color1 = interpolate(trail.color, t1);
				auto width1 = interpolate(trail.width, t1);
				float dist = (Vec2{v0.position.x, v0.position.y} - Vec2{v1.position.x, v1.position.y}).modulo();
				vertex[0] = {{v0.position.x, v0.position.y, v0.position.z-i*0.001}, {accUv, 0.}, texture, adjacentPoint0, {color0[0], color0[1], color0[2], color0[3]}, width0};
				accUv += dist;
				vertex[1] = {{v1.position.x, v1.position.y, v1.position.z-i*0.001}, {accUv, 0.}, texture, adjacentPoint1, {color1[0], color1[1], color1[2], color1[3]}, width1};
				batchRendererTrails->appendElement(vertex);
				adjacentPoint0 = {v0.position.x, v0.position.y};
			}
		}else{
			for(int i = 0; i < queue.size(); i++){
				auto& v0 = queue[i-1];
				float t0 = 1. - (float)v0.time/((float)v0.maxTime);
				auto color0 = interpolate(trail.color, t0);
				auto width0 = interpolate(trail.width, t0);

				auto trans = glm::scale(v0.tr, glm::vec3(width0, width0, 1.f));
				glm::vec4 c = {color0[0], color0[1], color0[2], color0[3]};
				serviceContext->renderer2D->drawSprite(trans, (GLuint)texture, {0., 0., 1., 1.}, c, 1.);
			}
		}
	}

	batchRendererTrails->flush();
}

std::string_view TrailSystem::getName(){
	static const std::string str{"TrailSystem"};
	return str;
}

ISystem* TrailSystem::clone(){
	auto* trailSystem =  new TrailSystem();
	trailSystem->batchRendererTrails = batchRendererTrails;
	return trailSystem;
}

TrailSystem::~TrailSystem(){ }

