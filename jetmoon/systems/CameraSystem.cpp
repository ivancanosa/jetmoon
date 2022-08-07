#include "CameraSystem.hpp"
#include <assert.h>                        // for assert
#include <memory>
#include <iosfwd>                          // for string
#include <set>                             // for set
#include <tuple>                           // for tuple
#include <type_traits>                     // for remove_extent_t
#include "GL/glew.h"                       // for GL_R32I, GL_RED_INTEGER
#include "commonStructures/Camera.hpp"     // for Camera
#include "commonStructures/ConfigDS.hpp"   // for ConfigDS, GraphicsConfig
#include "components/CameraComponent.hpp"  // for CameraComponent
#include "components/Transform.hpp"        // for Transform, fromMatrix
#include "core/ServiceContext.hpp"         // for ServiceContext
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp> 
#include "opengl/Framebuffer.hpp"          // for Framebuffer, FramebufferDe...
#include "services/Renderer2D.hpp"         // for Renderer2D
#include "commonStructures/ConfigDS.hpp"         // for ConfigDS

#include <iostream>

ISystem* CameraSystem::clone() {
	return new CameraSystem();
}

std::string_view CameraSystem::getName(){
	static std::string str{"CameraSystem"};
	return str;
}

void CameraSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void CameraSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto& cameraComponent = world->getComponent<CameraComponent>(entity);
	int resolutionWidth = serviceContext->configDS->graphicsOptions.resolutionWidth;
	int resolutionHeight = serviceContext->configDS->graphicsOptions.resolutionHeight;
	FramebufferDefinition fbDef { resolutionWidth, resolutionHeight, true, {
		{GL_RGBA16F, GL_RGBA}, {GL_R32I, GL_RED_INTEGER}
	}};
	cameraComponent.framebuffer = std::make_shared<Framebuffer>(fbDef);
}

void CameraSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	if(worldContext->camera.activeCamera == entity){
		worldContext->camera.activeCamera = NullEntity;
		std::cout << "Removed camera" << std::endl;
	}
}

void CameraSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	glm::vec4 white{1.0, 1.0, 1.0, 0.7};
	auto camera = &worldContext->camera;
//	camera->ratioX = float(m_viewport[3])/float(m_viewport[2]);
	for(auto&& entity: this->mEntities){
		auto& cameraComponent = world->getComponent<CameraComponent>(entity);
		float w = (serviceContext->configDS->graphicsOptions.resolutionWidth);
		float h = (serviceContext->configDS->graphicsOptions.resolutionHeight);
		cameraComponent.framebuffer->resize(w, h);
		auto tr = world->reduceComponent<Transform>(camera->activeCamera);
		if(cameraComponent.active && camera->activeCamera != entity){
			if(camera->activeCamera != NullEntity){
				auto& cameraComponent2 = world->getComponent<CameraComponent>(camera->activeCamera);
				cameraComponent2.active = false;
			}
			camera->activeCamera = entity;
		}
		float ratioX = cameraComponent.scale * ((float)cameraComponent.framebuffer->width / cameraComponent.framebuffer->height);
		float ratioY = cameraComponent.scale;
		glm::vec4 p0 = {ratioX*(-0.5+tr.position.x), ratioY*(0.5+tr.position.y), 0.9, 1.};
		glm::vec4 p1 = {ratioX*(0.5+tr.position.x), ratioY*(0.5+tr.position.y), 0.9, 1.};
		glm::vec4 p2 = {ratioX*(0.5+tr.position.x), ratioY*(-0.5+tr.position.y), 0.9, 1.};
		glm::vec4 p3 = {ratioX*(-0.5+tr.position.x), ratioY*(-0.5+tr.position.y), 0.9, 1.};
		serviceContext->renderer2D->drawLine(p0, p1, white);
		serviceContext->renderer2D->drawLine(p1, p2, white);
		serviceContext->renderer2D->drawLine(p2, p3, white);
		serviceContext->renderer2D->drawLine(p3, p0, white);
	}
	serviceContext->renderer2D->flushLines();

	if(camera->activeCamera != NullEntity){
		assert(world->hasComponent<CameraComponent>(camera->activeCamera) && "The camera has not a CameraComponent");
		auto trMatrix = world->reduceTransformMatrix(camera->activeCamera);
		auto tr = fromMatrix(trMatrix);
		auto& cameraComponent = world->getComponent<CameraComponent>(camera->activeCamera);
		camera->position = tr.position;
		camera->rotation = tr.rotation;
		camera->scale = cameraComponent.scale;
	}
}
