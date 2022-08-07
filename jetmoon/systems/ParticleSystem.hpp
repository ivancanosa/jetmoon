#pragma once
#include <GL/glew.h>                  // for GLuint
#include <memory>
#include <stddef.h>                   // for size_t
#include <array>                      // for array
#include <string_view>                // for string_view
#include "core/ISystem.hpp"           // for ISystem
#include "core/definitions.hpp"     
#include "core/interpolation.hpp"    
#include "glm/detail/type_vec3.hpp"   // for vec
#include "glm/detail/type_vec4.hpp"   // for vec
#include "glm/ext/vector_float3.hpp"  // for vec3
#include "glm/ext/vector_float4.hpp"  // for vec4
#include "opengl/BatchRenderer.hpp"   // for BatchRenderer (ptr only), POINT
#include "opengl/Shader.hpp"          // for Shader
class World;
struct GraphicsStatistics;
struct ParticleEmisor;
struct ServiceContext;
struct Transform;
struct WorldContext;

struct ParticleData{
	Vec3 pos{};
	GLuint textureIndex{0};
	float lifetime{1.}; // The lifetime is in seconds
	float actualLifetime{0.};
	float rotation{0.};

	VecInterpolation<Vec2> speed{};
	VecInterpolation<float> rotationSpeed{};
	VecInterpolation<float> size{};
	VecInterpolation<Vec4> color{};
};

struct ParticleVertex{
	glm::vec3 position;
	float size;
	float rotation;
	glm::vec4 color;
	float textureIndex;
};

class ParticleSystem: public ISystem{
	public:
		ParticleSystem(){}
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 
		void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		std::string_view getName() override;
		ISystem* clone() override;

	private:
		void clear();

		void createParticles(World* world, WorldContext* worldContext, ServiceContext* serviceContext);
		void updateParticles(World* world, WorldContext* worldContext, ServiceContext* serviceContext);

		ParticleData generateParticle(const ParticleEmisor& particleEmisor, const Transform& tr, GLuint textureId, ServiceContext* serviceContext);
		void insertParticleToArray(const ParticleData& data);
		void removeParticle(unsigned int position);

		VecInterpolation<Vec2> randomVecInterpolation(const VecInterpolation<Vec2VarRotation> vecInterpolation){
			return {randomVec(vecInterpolation.origin), randomVec(vecInterpolation.destiny), vecInterpolation.interpolation};
		}
		template <class T>
		VecInterpolation<T> randomVecInterpolation(const VecInterpolation<VecVar<T>>& vecInterpolation){
			return {randomVec(vecInterpolation.origin), randomVec(vecInterpolation.destiny), vecInterpolation.interpolation};
		}

		std::size_t particlesCount{0};
		Shader particleShader;
		GraphicsStatistics* graphicsStatistics{};

		std::array<ParticleData, MAX_PARTICLES> particlesArray;
		std::shared_ptr<BatchRenderer<ParticleVertex, 32768, RenderElement::POINT>> batchRendererParticles;
};
