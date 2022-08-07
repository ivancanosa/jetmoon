#pragma once
#include <GL/glew.h>                    // for GLuint
#include <stdint.h>                     // for int32_t, uint32_t
#include "core/definitions.hpp"         // for NullEntity, Entity
#include <glm/glm.hpp>  
#include "opengl/BatchRenderer.hpp"     // for BatchRenderer, RenderElement
#include "opengl/Shader.hpp"            // for Shader
class ShaderLoader;
struct GraphicsStatistics;
struct Spline2D;

const uint32_t BATCH_MAX_ELEMENTS = 512;

struct SpriteVertex{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float textureIndex;
	glm::vec4 textureClip;
	float tilingFactor;
	int32_t entity;
};

struct SplineVertex{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float despl{0.};
	glm::vec4 coefficientsX;
	glm::vec4 coefficientsY;
	float thickness;
	float textureIndex;
	float tilingFactor;
	int32_t entity;
};

struct LineVertex{
	glm::vec3 position;
	glm::vec2 texCoord;
	float textureIndex;
	glm::vec4 color;
	float width;
};

struct DebugLineVertex{
	glm::vec3 position;
	glm::vec4 color;
};

class Renderer2D{
public:
	void init(ShaderLoader& loader, GraphicsStatistics* graphicStatisticsPtr);
	void setCameraTransform(const glm::mat4& viewTransform);
	void drawDebugLine(const glm::vec3& origin, const glm::vec3& destiny, const glm::vec4& color={1.,1.,1.,1.});
	void drawLine(const glm::vec3& origin, const glm::vec3& destiny, const glm::vec4& color={1.,1.,1.,1.}, float width=0.001, GLuint textureID=0);
	
	void drawSpline(Spline2D& spline, GLuint texture, float thickness=0.1, const glm::vec4& color={1.,1.,1.,1.}, float tilingFactor=1.);

	void drawCircle(const glm::vec3& origin, float radius, GLuint circleId);

	void drawSquad(const glm::vec3& position, const glm::vec2& size, float rotation=0., Entity entity=NullEntity);
	void drawSquad(const glm::mat4& transform, Entity entity=NullEntity);

	void drawSprite(const glm::mat4& transform, GLuint textureID, Entity entity=NullEntity);
	void drawSprite(const glm::mat4& transform, GLuint textureID, const glm::vec4& textureClip, glm::vec4& color, float tiling=1.0f, Entity entity=NullEntity);

	void flushLines();
	void flushDebugLines();
	void flushSprites();
	void flushSplines();
	void flushAll();
private:
	void initSpriteVAO();
	void initLineVAO();
	void initDebugLineVAO();
	void initSplineVAO();

	GraphicsStatistics* graphicsStatistics{};
	Shader spriteShader, debugLineShader, lineShader, splineShader;
	GLuint whiteTextureId;

	BatchRenderer<SpriteVertex, BATCH_MAX_ELEMENTS, RenderElement::SQUAD> batchRendererSprites;
	BatchRenderer<LineVertex,   BATCH_MAX_ELEMENTS, RenderElement::LINE> batchRendererLines;
	BatchRenderer<DebugLineVertex,   BATCH_MAX_ELEMENTS, RenderElement::LINE> batchRendererDebugLines;
	BatchRenderer<SplineVertex, BATCH_MAX_ELEMENTS, RenderElement::LINE> batchRendererSplines;
};

