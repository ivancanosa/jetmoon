#include "core/ISystem.hpp"
#include "core/World.hpp"
#include "systems/SaveEntityMapSystem.hpp"
#include "opengl/Framebuffer.hpp"
#include "opengl/Texture.hpp"
#include "services/EditorDS.hpp"
#include "services/RenderContext.hpp"

#include "core/ServiceContext.hpp"



void SaveEntityMapSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
}

void SaveEntityMapSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void SaveEntityMapSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void SaveEntityMapSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	std::shared_ptr<Texture> texture{};
	if(serviceContext->editorDS->entityMapTexture == nullptr){
		auto framebuffer = 	serviceContext->renderContext->actualFramebuffer;
		if(framebuffer->attachmentsCount >= 2){
			texture = framebuffer->texturesArray[1]->clone();
			serviceContext->editorDS->entityMapTexture = texture;
		}
	}else{
		texture = serviceContext->editorDS->entityMapTexture;
		auto textureCamera = serviceContext->renderContext->actualFramebuffer->texturesArray[1];
		texture->resize(textureCamera->width, textureCamera->height);
		texture->copy(textureCamera);
	}
}

std::string_view SaveEntityMapSystem::getName(){
	static std::string str{"SaveEntityMapSystem"};
	return str;
}

ISystem* SaveEntityMapSystem::clone(){
	return new SaveEntityMapSystem();
}

SaveEntityMapSystem::~SaveEntityMapSystem(){ }

