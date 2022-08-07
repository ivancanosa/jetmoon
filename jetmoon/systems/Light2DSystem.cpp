#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "systems/Light2DSystem.hpp"
#include <stddef.h>                               // for offsetof
#include <array>                                  // for array
#include <iosfwd>                                 // for string
#include <set>                                    // for set
#include <type_traits>                            // for remove_extent_t
#include "commonStructures/Camera.hpp"            // for Camera
#include "commonStructures/EngineStatistics.hpp"  // for EngineStatistics
#include "commonStructures/LightningConfig.hpp"   // for LightningConfig
#include "components/CameraComponent.hpp"         // for CameraComponent
#include "components/LightSource.hpp"             // for LightSource
#include "core/ServiceContext.hpp"                // for ServiceContext
#include "core/World.hpp"                         // for World
#include "core/WorldContext.hpp"                  // for WorldContext
#include "opengl/Framebuffer.hpp"                 // for Framebuffer
#include "opengl/Texture.hpp"                 // for Framebuffer
#include "opengl/Shader.hpp"                      // for Shader
#include "services/RenderContext.hpp"             // for RenderContext
#include "services/ShaderLoader.hpp"              // for ShaderLoader
#include "utils/GLUtils.hpp"                      // for glGenerateUnityVertex

bool Light2DSystem::initializedFb = false;
std::shared_ptr<Framebuffer> Light2DSystem::maskFb = nullptr;
std::shared_ptr<Framebuffer> Light2DSystem::auxFb = nullptr;
GLuint Light2DSystem::VAO = 0;
GLuint Light2DSystem::VBO = 0;
GLuint Light2DSystem::EBO = 0;

void Light2DSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	auto lightShader = serviceContext->shaderLoader->getShader("lightShader");
	batchRendererLights = std::make_shared<BatchRenderer<LightVertex, BATCH_MAX_ELEMENTS, RenderElement::SQUAD>>();
	batchRendererLights->init(lightShader, {
		{GL_FLOAT, 3, offsetof(LightVertex, position)},
		{GL_FLOAT, 3, offsetof(LightVertex, color)},
		{GL_FLOAT, 2, offsetof(LightVertex, uv)},
		{GL_FLOAT, 1, offsetof(LightVertex, intensity)},
	});
	batchRendererLights->setGraphicsStatistics(&(serviceContext->engineStatistics->graphicsStatistics));
}

void Light2DSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) { }

void Light2DSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void Light2DSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	bool lightMask = worldContext->lightningConfig.lightMask;
	LightVertex lightVertex[4];
	static constexpr glm::vec4 originVertexPos[4] = {
		{ 0.5,  0.5, 0., 1.},
		{ 0.5, -0.5, 0., 1.},
		{-0.5, -0.5, 0., 1.},
		{-0.5,  0.5, 0., 1.}
	};
	static constexpr glm::vec2 uv[4] = {
		{1.0, 1.0 },
		{1.0, 0.0 },
		{0.0, 0.0 },
		{0.0, 1.0 }
	};
	batchRendererLights->setViewTransform(worldContext->camera.getTransformationMatrix());

	glm::mat4 trans;

	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	Entity activeCamera = worldContext->camera.activeCamera;
	std::shared_ptr<Framebuffer> originalFb = serviceContext->renderContext->actualFramebuffer;
	if(activeCamera != NullEntity){
		originalFb = world->getComponent<CameraComponent>(activeCamera).framebuffer;
	}else{
		originalFb = serviceContext->renderContext->actualFramebuffer;
	}
	if(lightMask){
		if(!initializedFb){
			initializedFb = true;
			auto[a, b, c] = glGenerateUnityVertex();
			VAO = a;
			VBO = b;
			EBO = c;
			maskFb = std::make_shared<Framebuffer>(originalFb->fbDef);
			auxFb = std::make_shared<Framebuffer>(originalFb->fbDef);
		}
		maskFb->resize(originalFb->width, originalFb->height);
		auxFb->resize(originalFb->width, originalFb->height);
		maskFb->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	for(auto&& entity: this->mEntities){
		auto lightSource = world->getComponent<LightSource>(entity);
		auto tr = world->reduceTransformMatrix(entity);
		trans = glm::scale(tr, glm::vec3(lightSource.radius, lightSource.radius, 1.f));
		for(int i=0; i<4; i++){
			lightVertex[i].position = trans * originVertexPos[i];
			lightVertex[i].color[0] = lightSource.color[0];
			lightVertex[i].color[1] = lightSource.color[1];
			lightVertex[i].color[2] = lightSource.color[2];
			lightVertex[i].uv = uv[i];
			lightVertex[i].intensity = lightSource.intensity;
		}
		batchRendererLights->appendElement(lightVertex);
	}
	batchRendererLights->flush();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); 

	if(lightMask){
		glBindVertexArray(VAO);
		auxFb->bind();
		auto maskShader = serviceContext->shaderLoader->getShader("maskTexture");
		maskShader.bind();
		maskShader.setUniform("backgroundColor", worldContext->lightningConfig.backgroundColor);
		originalFb->texturesArray[0]->bindAsUnit(0);
		maskFb->texturesArray[0]->bindAsUnit(1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		serviceContext->shaderLoader->getShader("renderToWindow").bind();
		originalFb->bind();
		auxFb->texturesArray[0]->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

std::string_view Light2DSystem::getName(){
	static std::string str{"Light2DSystem"};
	return str;
}

ISystem* Light2DSystem::clone(){
	Light2DSystem* system = new Light2DSystem();
	system->batchRendererLights = batchRendererLights;
	return system;
}

Light2DSystem::~Light2DSystem(){ }

