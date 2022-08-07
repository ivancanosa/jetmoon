#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"
#include "systems/ParticleSystem.hpp"
#include <assert.h>                               // for assert
#include <math.h>                                 // for cos, sin
#include <iostream>                               // for string
#include <set>                                    // for set
#include <type_traits>                            // for remove_extent_t
#include <variant>                                // for visit
#include "GL/glew.h"                              // for glDepthFunc, GL_FLOAT
#include "commonStructures/Camera.hpp"            // for Camera
#include "commonStructures/EngineStatistics.hpp"  // for EngineStatistics
#include "components/ParticleEmisor.hpp"          // for ParticleEmisor
#include "components/Transform.hpp"               // for Transform
#include "core/ServiceContext.hpp"                // for ServiceContext
#include "core/Time.hpp"                          // for Time, Time::deltaTime
#include "core/World.hpp"                         // for World
#include "core/WorldContext.hpp"                  // for WorldContext
#include "core/definitions.hpp"                   
#include "core/interpolation.hpp"                
#include "services/ImageLoader.hpp"               // for ImageLoader
#include "services/ShaderLoader.hpp"              // for ShaderLoader
#include "utils/Shapes.hpp"                       // for RectangularShape



void ParticleSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	particleShader = serviceContext->shaderLoader->getShader("particlesShader");
	graphicsStatistics = &serviceContext->engineStatistics->graphicsStatistics;
	batchRendererParticles = std::make_shared<BatchRenderer<ParticleVertex, 32768, RenderElement::POINT>>();
	batchRendererParticles->init(particleShader, {
		{GL_FLOAT, 3, offsetof(ParticleVertex, position)},
		{GL_FLOAT, 1, offsetof(ParticleVertex, size)},
		{GL_FLOAT, 1, offsetof(ParticleVertex, rotation)},
		{GL_FLOAT, 4, offsetof(ParticleVertex, color)},
		{GL_FLOAT, 1, offsetof(ParticleVertex, textureIndex)},
	});
	batchRendererParticles->setGraphicsStatistics(graphicsStatistics);
}

void ParticleSystem::addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {

}

void ParticleSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {

}

//TODO: WARNING, all particles created die one frame early
void ParticleSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	batchRendererParticles->setViewTransform(worldContext->camera.getTransformationMatrix());
	createParticles(world, worldContext, serviceContext);
	updateParticles(world, worldContext, serviceContext);
}

Vec3 randomPoint(const PointShape& _){
	return Vec3{0.,0.,0.};
}

Vec3 randomPoint(const RectangularShape& rectangularShape){
	return Vec3{
		randomNumberCentered(0., rectangularShape.width),
		randomNumberCentered(0., rectangularShape.height),
		0.
	};
}

Vec3 randomPoint(const CircularShape& circularShape){
	float beta = randomNumberBetween(0., 2.*glm::pi<float>());
	Vec2 vec = {randomNumberBetween(0., circularShape.radio), 0.};
	return Vec3{
		(vec.x*cos(beta) - vec.y*sin(beta)),
		(vec.x*sin(beta) + vec.y*cos(beta)),
		0.
	};
}

Vec3 randomPoint(const RingShape& ringShape){
	float beta = randomNumberBetween(0., 2.*glm::pi<float>());
	Vec2 vec {randomNumberBetween(ringShape.innerRadio, ringShape.outerRadio), 0.};
	return Vec3{
		(vec.x*cos(beta) - vec.y*sin(beta)),
		(vec.x*sin(beta) + vec.y*cos(beta)),
		0.
	};
}

Vec3 randomPoint(const ShapeVariant& shapeVariant){
	return std::visit([](auto&& arg){return randomPoint(arg);}, shapeVariant);
}

ParticleData ParticleSystem::generateParticle(const ParticleEmisor& particleEmisor, const Transform& tr, GLuint textureId, ServiceContext* serviceContext){
	ParticleData particleData{};
	particleData.pos =  randomPoint(particleEmisor.spawnShape) + tr.position;
	particleData.textureIndex = serviceContext->imageLoader->load(particleEmisor.texture);
	particleData.lifetime = randomVec(particleEmisor.lifetime);
	particleData.rotation = randomVec(particleEmisor.rotation);
	particleData.rotationSpeed = randomVecInterpolation(particleEmisor.rotationSpeed);
	particleData.size = randomVecInterpolation(particleEmisor.size);
	particleData.color = randomVecInterpolation(particleEmisor.color);
	particleData.speed = randomVecInterpolation(particleEmisor.speed);
	
	return particleData;
}

void ParticleSystem::insertParticleToArray(const ParticleData& data){
	assert(particlesCount <= (MAX_PARTICLES-1) && "The max number of particles has been exceeded");
	particlesArray[particlesCount] = data;
	particlesCount++;
}

void ParticleSystem::clear(){
	particlesCount = 0;
}


//TODO: Sort particles by z
void ParticleSystem::updateParticles(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	unsigned int i = 0;
	float dt = fmin(0.33, 0.000001 * float(Time::deltaTime));
	ParticleVertex particleVertex;

	glDepthFunc( GL_LEQUAL);
	while(i < particlesCount){
		auto& particle = particlesArray[i];
		particle.actualLifetime += dt;
		if(particle.actualLifetime >= particle.lifetime){
			removeParticle(i);
			continue;
		}
		float t = particle.actualLifetime / particle.lifetime;

		Vec2 speed = interpolate(particle.speed, t);
		float rotationSpeed = interpolate(particle.rotationSpeed, t);
		float size = interpolate(particle.size, t);
		Vec4 color = interpolate(particle.color, t);
		
		particle.pos.x += (speed * dt).x;
		particle.pos.y += (speed * dt).y;
		particle.rotation += rotationSpeed * dt;

		particleVertex.position = glm::vec3(particle.pos.x, particle.pos.y , particle.pos.z);
		particleVertex.size = size;
		particleVertex.rotation = particle.rotation;

		particleVertex.color[0] = color[0];
		particleVertex.color[1] = color[1];
		particleVertex.color[2] = color[2];
		particleVertex.color[3] = color[3];

		particleVertex.textureIndex = particle.textureIndex;

		batchRendererParticles->appendElement(&particleVertex);

		i += 1;
	}

	batchRendererParticles->flush();
	glDepthFunc(GL_LESS);
	graphicsStatistics->particlesCount = particlesCount;
}

void ParticleSystem::removeParticle(unsigned int position){
	assert(position >= 0 && position <= (particlesCount-1) && "Tried to remove an incorret particle position of the array");
	particlesCount -= 1;
	this->particlesArray[position] = this->particlesArray[particlesCount];
}

//TODO: Create particles only if they are inside or close to the camera area
void ParticleSystem::createParticles(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	ParticleData particleData{};
	float dt = fmin(0.33, 0.000001 * float(Time::deltaTime));
	for(auto& entity: this->mEntities){
		auto& particleEmisor = world->getComponent<ParticleEmisor>(entity);
		GLuint textureId = serviceContext->imageLoader->load(particleEmisor.texture);

		//Continuous generation
		if(particleEmisor.continuous){
			auto tr = world->reduceComponent<Transform>(entity);
			particleEmisor.deltaTimeAcc += dt;
			while(particleEmisor.deltaTimeAcc > 1.0 / particleEmisor.continuousGenerationRate){
				particleData = generateParticle(particleEmisor, tr, textureId, serviceContext);
				insertParticleToArray(particleData);
				particleEmisor.deltaTimeAcc -= 1.0 / particleEmisor.continuousGenerationRate;
			}
		}

		//Burst Generation
		if(particleEmisor.burst){
			auto tr = world->reduceComponent<Transform>(entity);
			for(int i = 0; i< particleEmisor.burstCount; i++){
				particleData = generateParticle(particleEmisor, tr, textureId, serviceContext);
				insertParticleToArray(particleData);
			}
			particleEmisor.burst = false;
		}
	}
}


std::string_view ParticleSystem::getName(){
	static std::string str{"ParticleSystem"};
	return str;
}

ISystem* ParticleSystem::clone(){
	ParticleSystem* system = new ParticleSystem();
	system->particleShader = particleShader;
	system->particlesCount = 0;
	system->batchRendererParticles = batchRendererParticles;
	system->graphicsStatistics = graphicsStatistics;
	return system;
}
