#include "VolumetricPostProcessingSystem.hpp"
#include <iosfwd>                          // for string
#include <set>                             // for set
#include <vector>                          // for vector
#include "commonStructures/Camera.hpp"     // for Camera
#include "components/CameraComponent.hpp"  // for CameraComponent
#include "components/CollisionSensor.hpp"  // for CollisionSensor, Collision...
#include "components/PostProcessing.hpp"   // for PostProcessing
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext
#include "core/definitions.hpp"            // for Entity
struct ServiceContext;



std::string_view VolumetricPostProcessingSystem::getName(){
	static std::string str{"VolumetricPostProcessingSystem"};
	return str;
}

ISystem* VolumetricPostProcessingSystem::clone(){
	auto newPost = new VolumetricPostProcessingSystem();
	newPost->postProcessingHelper = postProcessingHelper;
	return newPost;
}

void VolumetricPostProcessingSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void VolumetricPostProcessingSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	Entity activeCamera = worldContext->camera.activeCamera;
	bool apply = false;
	for(auto entity: this->mEntities){
		auto& sensor = world->getComponent<CollisionSensor>(entity);
		for(auto camera: sensor.stayingEntities){
			if(camera.entity == activeCamera){
				apply = true;
				break;
			}
		}
		if(apply){
			auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
			auto& postProcessing = world->getComponent<PostProcessing>(entity);
			postProcessingHelper.exec(entity, cameraComponent.framebuffer, postProcessing.postProcessingPipeline, serviceContext);
		}
		apply = false;
	}
}
