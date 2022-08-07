#include "Renderer2D.hpp"
#include <GL/glew.h>                              // for GL_FLOAT, GLuint
#include <stddef.h>                               // for offsetof
#include <initializer_list>                       // for initializer_list
#include <vector>                                 // for vector
#include "commonStructures/EngineStatistics.hpp"  // for GraphicsStatistics
#include <glm/glm.hpp>  
#include "services/ShaderLoader.hpp"              // for ShaderLoader
#include "utils/Spline.hpp"                       // for SplineCoefficients



void Renderer2D::drawSpline(Spline2D& spline, GLuint texture, float thickness, const glm::vec4& color, float tilingFactor){
	auto& points = spline.points;

	float lengthAcc = 0.;
	for(int i=0; i<points.size() - 1; i++){
		float uv0 = lengthAcc/thickness;
		lengthAcc += spline.getArcLenght(i, 10);
		float uv1 = lengthAcc/thickness;

		auto x0 = points[i].x;
		auto x1 = points[i+1].x;
		float y0 = points[i].y;
		float y1 = points[i+1].y;

		SplineVertex splineVertex[2];
		splineVertex[0].position = {x0, y0, 1.};
		splineVertex[0].color = color;
		splineVertex[0].texCoord = {uv0, 0.};
		splineVertex[0].despl = i;
		splineVertex[0].coefficientsX = spline.coeficientsX[i].getGLM();
		splineVertex[0].coefficientsY = spline.coeficientsY[i].getGLM();
		splineVertex[0].thickness = thickness;
		splineVertex[0].textureIndex = texture;
		splineVertex[0].tilingFactor = tilingFactor;
		splineVertex[0].entity = NullEntity;

		splineVertex[1].position = {x1, y1, 1.};
		splineVertex[1].color = color;
		splineVertex[1].texCoord = {uv1, 0.};
		splineVertex[1].despl = i;
		splineVertex[1].coefficientsX = spline.coeficientsX[i].getGLM();
		splineVertex[1].coefficientsY = spline.coeficientsY[i].getGLM();
		splineVertex[1].thickness = thickness;
		splineVertex[1].textureIndex = texture;
		splineVertex[1].tilingFactor = tilingFactor;
		splineVertex[1].entity = NullEntity;

		batchRendererSplines.appendElement(splineVertex);
	}
}

void Renderer2D::initDebugLineVAO(){
	batchRendererDebugLines.init(debugLineShader, {
		{GL_FLOAT, 3, offsetof(DebugLineVertex, position)},
		{GL_FLOAT, 4, offsetof(DebugLineVertex, color)}
	});
	batchRendererDebugLines.setGraphicsStatistics(graphicsStatistics);
}

void Renderer2D::initSpriteVAO(){
	batchRendererSprites.init(spriteShader, {
		{GL_FLOAT, 3, offsetof(SpriteVertex, position)},
		{GL_FLOAT, 4, offsetof(SpriteVertex, color)},
		{GL_FLOAT, 2, offsetof(SpriteVertex, texCoord)},
		{GL_FLOAT, 1, offsetof(SpriteVertex, textureIndex)},
		{GL_FLOAT, 4, offsetof(SpriteVertex, textureClip)},
		{GL_FLOAT, 1, offsetof(SpriteVertex, tilingFactor)},
		{GL_INT, 1, offsetof(SpriteVertex, entity)}
	});
	batchRendererSprites.setGraphicsStatistics(graphicsStatistics);
}

void Renderer2D::initLineVAO(){
	batchRendererLines.init(lineShader, {
		{GL_FLOAT, 3, offsetof(LineVertex, position)},
		{GL_FLOAT, 2, offsetof(LineVertex, texCoord)},
		{GL_FLOAT, 1, offsetof(LineVertex, textureIndex)},
		{GL_FLOAT, 4, offsetof(LineVertex, color)},
		{GL_FLOAT, 1, offsetof(LineVertex, width)}
	});
	batchRendererLines.setGraphicsStatistics(graphicsStatistics);
}

void Renderer2D::initSplineVAO(){
	batchRendererSplines.init(splineShader, {
		{GL_FLOAT, 3, offsetof(SplineVertex, position)},
		{GL_FLOAT, 4, offsetof(SplineVertex, color)},
		{GL_FLOAT, 2, offsetof(SplineVertex, texCoord)},
		{GL_FLOAT, 1, offsetof(SplineVertex, despl)},
		{GL_FLOAT, 4, offsetof(SplineVertex, coefficientsX)},
		{GL_FLOAT, 4, offsetof(SplineVertex, coefficientsY)},
		{GL_FLOAT, 1, offsetof(SplineVertex, thickness)},
		{GL_FLOAT, 1, offsetof(SplineVertex, textureIndex)},
		{GL_FLOAT, 1, offsetof(SplineVertex, tilingFactor)},
		{GL_INT, 1, offsetof(SplineVertex, entity)}
	});
	batchRendererSplines.setGraphicsStatistics(graphicsStatistics);
}

void Renderer2D::init(ShaderLoader& loader, GraphicsStatistics* graphicStatisticsPtr){
	spriteShader = loader.getShader("spriteShader");
	lineShader = loader.getShader("lineShader");
	debugLineShader = loader.getShader("debugLineShader");
	splineShader = loader.getShader("splineShader");
	graphicsStatistics = graphicStatisticsPtr;

	initSpriteVAO();
	initLineVAO();
	initDebugLineVAO();
	initSplineVAO();
}

void Renderer2D::setCameraTransform(const glm::mat4& viewTransform){
	batchRendererSprites.setViewTransform(viewTransform);
	batchRendererLines.setViewTransform(viewTransform);
	batchRendererDebugLines.setViewTransform(viewTransform);
	batchRendererSplines.setViewTransform(viewTransform);
}

//======================== Lines Rendering ==============================

void Renderer2D::drawDebugLine(const glm::vec3& origin, const glm::vec3& destiny, const glm::vec4& color){
	DebugLineVertex lineVertex[2];
	lineVertex[0].position = origin;
	lineVertex[0].color = color;
	lineVertex[1].position = destiny;
	lineVertex[1].color = color;
	batchRendererDebugLines.appendElement(lineVertex);
}

void Renderer2D::drawLine(const glm::vec3& origin, const glm::vec3& destiny, const glm::vec4& color, float width, GLuint textureID){
	LineVertex lineVertex[2];
	if(textureID == 0) textureID = whiteTextureId;

	lineVertex[0].position = origin;
	lineVertex[0].texCoord = {0., 0.};
	lineVertex[0].textureIndex = textureID;
	lineVertex[0].color = color;
	lineVertex[0].width = width;
	lineVertex[1].position = destiny;
	lineVertex[1].texCoord = {1., 0.};
	lineVertex[1].textureIndex = textureID;
	lineVertex[1].color = color;
	lineVertex[1].width = width;

	batchRendererLines.appendElement(lineVertex);
}

//======================== Squad Rendering ==============================

void Renderer2D::drawCircle(const glm::vec3& origin, float radius, GLuint circleId){
	auto trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(origin.x, origin.y, origin.z));
	trans = glm::scale(trans, glm::vec3(2*radius, 2*radius, 1.f));
	drawSprite(trans, circleId);
	graphicsStatistics->circlesRendered += 1;
}

void Renderer2D::drawSquad(const glm::vec3& position, const glm::vec2& size, float rotation, Entity entity){
}

void Renderer2D::drawSquad(const glm::mat4& transform, Entity entity){
	glm::vec4 textureClip{0., 0., 1., 1.};
	glm::vec4 color{1.,1.,1.,1.};
	drawSprite(transform, whiteTextureId, textureClip, color);
}

void Renderer2D::drawSprite(const glm::mat4& transform, GLuint textureID, Entity entity){
	glm::vec4 textureClip{0., 0., 1., 1.};
	glm::vec4 color{1.,1.,1.,1.};
	drawSprite(transform, textureID, textureClip, color, 1.f, entity);
}

void Renderer2D::drawSprite(const glm::mat4& transform, GLuint textureID, const glm::vec4& textureClip, glm::vec4& color, float tiling, Entity entity){
	SpriteVertex spriteVertex[4];
	constexpr glm::vec4 originVertexPos[4] = {
		{ 0.5,  0.5, 0., 1.},
		{ 0.5, -0.5, 0., 1.},
		{-0.5, -0.5, 0., 1.},
		{-0.5,  0.5, 0., 1.}
	};
	constexpr glm::vec2 originTextureCoord[4] = {
		{1.0, 1.0 },
		{1.0, 0.0 },
		{0.0, 0.0 },
		{0.0, 1.0 }
	};

	for(int i=0; i<4; i++){
		spriteVertex[i].position = transform * originVertexPos[i];
		spriteVertex[i].color = color;
		spriteVertex[i].texCoord = originTextureCoord[i];
		spriteVertex[i].textureIndex = textureID;
		spriteVertex[i].textureClip = textureClip;
		spriteVertex[i].tilingFactor = tiling;
		spriteVertex[i].entity = entity + 1;
	}

	batchRendererSprites.appendElement(spriteVertex);
}

void Renderer2D::flushSplines(){
	batchRendererSplines.flush();
}

void Renderer2D::flushDebugLines(){
	batchRendererDebugLines.flush();
}

void Renderer2D::flushSprites(){
	batchRendererSprites.flush();
}

void Renderer2D::flushLines(){
	batchRendererLines.flush();
}

void Renderer2D::flushAll(){
	flushSprites();
	flushLines();
	flushDebugLines();
	flushSplines();
}
