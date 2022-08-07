#include "ProceduralTextureSystem.hpp"
#include <iosfwd>                            // for string
#include <set>                               // for set
#include <tuple>                             // for tuple
#include <type_traits>                       // for remove_extent_t, is_base_of
#include <variant>                           // for visit
#include "commonStructures/Camera.hpp"       // for Camera
#include "components/CameraComponent.hpp"    // for CameraComponent
#include "components/ProceduralTexture.hpp"  // for ProceduralTexture
#include "core/ServiceContext.hpp"           // for ServiceContext
#include "core/Time.hpp"                     // for Time, Time::fromLaunch
#include "core/World.hpp"                    // for World
#include "core/WorldContext.hpp"             // for WorldContext
#include "opengl/Framebuffer.hpp"            // for Framebuffer, Framebuffer...
#include "opengl/Texture.hpp"            // for Framebuffer, Framebuffer...
#include "services/RenderContext.hpp"        // for RenderContext
#include "services/Renderer2D.hpp"           // for Renderer2D
#include "services/ShaderLoader.hpp"         // for ShaderLoader
#include "utils/GLUtils.hpp"                 // for glGenerateUnityVertex
#include "utils/visit_struct.hpp"            // for for_each
struct TypedShaderGraph;


std::string_view ProceduralTextureSystem::getName(){
	static std::string str{"ProceduralTextureSystem"};
	return str;
}

ISystem* ProceduralTextureSystem::clone(){
	auto newProc = new ProceduralTextureSystem();
	newProc->framebuffersArray = framebuffersArray;
	newProc->init(nullptr, nullptr, nullptr);
	return newProc;
}

void ProceduralTextureSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto[a, b, c] = glGenerateUnityVertex();
	VAO = a;
	VBO = b;
	EBO = c;
}

void ProceduralTextureSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	auto& c = world->getComponent<ProceduralTexture>(entity);
	FramebufferDefinition fbDef{ (GLuint)c.width, (GLuint)c.height, false, {{GL_RGBA16F, GL_RGBA}}}; 
	framebuffersArray[entity] = std::make_shared<Framebuffer>(fbDef);
}

void ProceduralTextureSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	framebuffersArray[entity] = nullptr;
}

void ProceduralTextureSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	float u_Time = Time::fromLaunch/1000000.;
	glBindVertexArray(VAO);
	for(auto entity: this->mEntities){
		auto& component = world->getComponent<ProceduralTexture>(entity);
		if(!component.dynamic) continue;
		auto fb = framebuffersArray[entity];
		fb->resize(component.width, component.height);
		fb->bind();
		std::visit([&](auto&& arg){
					if constexpr(!std::is_base_of<TypedShaderGraph,typename std::remove_reference<decltype(arg)>::type>()){
						auto shader = serviceContext->shaderLoader->getShader(arg.shader);
						shader.bind();
						shader.setUniform("u_Time", u_Time);
						visit_struct::for_each(arg,
								[&](const char * name, const auto & value) {
								shader.setUniform(name, value);	
								});
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					}
					
				}, component.shader);
	}
	Entity activeCamera = worldContext->camera.activeCamera;
	if(activeCamera != NullEntity){
		auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
		cameraComponent.framebuffer->bind();
	}else{
#ifndef __RELEASE__
		serviceContext->renderContext->changeFramebuffer("editor");
#else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  
#endif
	}

	for(auto entity: this->mEntities){
		auto tr = world->reduceTransformMatrix(entity);
		serviceContext->renderer2D->drawSprite(tr, framebuffersArray[entity]->texturesArray[0]->textureId, entity);
	}
}

ProceduralTextureSystem::~ProceduralTextureSystem(){
	glDeleteVertexArrays(1, &VAO);
}
