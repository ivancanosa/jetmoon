#include "PostProcessingSystem.hpp"
#include <iosfwd>                          // for string
#include "commonStructures/Camera.hpp"     // for Camera
#include "components/CameraComponent.hpp"  // for CameraComponent
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext
#include "core/definitions.hpp"            // for Entity, NullEntity
struct ServiceContext;


std::string_view PostProcessingSystem::getName(){
	static std::string str{"PostProcessingSystem"};
	return str;
}

ISystem* PostProcessingSystem::clone(){
	auto newPost = new PostProcessingSystem();
	newPost->postProcessingHelper = postProcessingHelper;
	return newPost;
}

void PostProcessingSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void PostProcessingSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	Entity activeCamera = worldContext->camera.activeCamera;
	if(activeCamera != NullEntity){
		auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
		postProcessingHelper.exec(activeCamera, cameraComponent.framebuffer, cameraComponent.postProcessingPipeline, serviceContext);
	}
}
