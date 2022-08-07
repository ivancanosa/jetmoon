#pragma once
#include <GL/glew.h>                              // for GLuint, glDrawElements
#include <assert.h>                               // for assert
#include <stddef.h>                               // for size_t
#include <array>                                  // for array
#include <initializer_list>                       // for initializer_list
#include "commonStructures/EngineStatistics.hpp"  // for GraphicsStatistics
#include "glm/ext/matrix_float4x4.hpp"            // for mat4
#include "opengl/GLdefinitions.hpp"               // for has_texture
#include "opengl/Shader.hpp"                      // for Shader

enum RenderElement{
	POINT, LINE, TRIANGLE, SQUAD
};

constexpr int getVertexCount(RenderElement renderElement){
	switch(renderElement){
		case RenderElement::POINT:
			return 1;
		case RenderElement::LINE:
			return 2;
		case RenderElement::TRIANGLE:
			return 3;
		case RenderElement::SQUAD:
			return 4;
	}
	assert(false && "Error, Render mode not available");
}

struct VertexAtribute{
	GLuint gl_type;
	unsigned int count;
	std::size_t offset;
};

template <RenderElement renderElement>
void generateIndexBuffer(GLuint& EBO, std::size_t ElementsBufferSize){
	assert(false && "Error, renderMode no applicable");
}
template <>
void generateIndexBuffer<RenderElement::POINT>(GLuint& EBO, std::size_t ElementsBufferSize);
template <>
void generateIndexBuffer<RenderElement::LINE>(GLuint& EBO, std::size_t ElementsBufferSize);
template <>
void generateIndexBuffer<RenderElement::TRIANGLE>(GLuint& EBO, std::size_t ElementsBufferSize);
template <>
void generateIndexBuffer<RenderElement::SQUAD>(GLuint& EBO, std::size_t ElementsBufferSize);

template <typename VertexType, std::size_t ElementsBufferSize, RenderElement renderElement>
class BatchRenderer{
	public:
		void init(Shader shader, std::initializer_list<VertexAtribute> initializerList){
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), nullptr, GL_STATIC_DRAW);

			generateIndexBuffer<renderElement>(EBO, ElementsBufferSize);

			int i = 0;
			for(auto& atribute: initializerList){
				if(atribute.gl_type == GL_INT){
					glVertexAttribIPointer(i, atribute.count, GL_INT, sizeof(VertexType), reinterpret_cast<void*>(atribute.offset));
				}else{
					glVertexAttribPointer(i, atribute.count, atribute.gl_type, GL_FALSE, sizeof(VertexType), reinterpret_cast<void*>(atribute.offset));
				}
				glEnableVertexAttribArray(i);
				i += 1;
			}

			glBindVertexArray(0);
			this->shader = shader;
		}

		bool isFull(){
			return elementCounter >= ElementsBufferSize || textureCounter >= 32;
		}

		void flush(){
			if(elementCounter <= 0) return;

			shader.bind();
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, elementCounter * sizeof(VertexType) * vertexPerElementCount, vertexBuffer.data()); 

			for(int i=0; i < textureCounter; i++){
				glBindTextureUnit(i, textureSlots[i]);
			}

			switch(renderElement){
				case RenderElement::POINT:
					glDrawElements(GL_POINTS, 1 * elementCounter, GL_UNSIGNED_INT, 0);
					graphicsStatistics->pointsRendered += elementCounter;
					break;
				case RenderElement::LINE:
					glDrawElements(GL_LINES, 2 * elementCounter, GL_UNSIGNED_INT, 0);
					graphicsStatistics->linesRendered += elementCounter;
					break;
				case RenderElement::TRIANGLE:
					glDrawElements(GL_TRIANGLES, 3 * elementCounter, GL_UNSIGNED_INT, 0);
					graphicsStatistics->trianglesRendered += elementCounter;
					break;
				case RenderElement::SQUAD:
					glDrawElements(GL_TRIANGLES, 6 * elementCounter, GL_UNSIGNED_INT, 0);
					graphicsStatistics->quadsRendered += elementCounter;
					break;
			};

			glBindTexture(GL_TEXTURE_2D, 0);

			graphicsStatistics->drawCalls += 1;
			graphicsStatistics->texturesBinded += textureCounter;

			elementCounter = 0;
			textureCounter = 0;
		}

		void appendElement(VertexType* vertexStruct){
			flushIfFull();

			if constexpr(has_texture<VertexType>::value){
				GLuint textureId = (GLuint)vertexStruct[0].textureIndex;
				float textureIdFloat{1.};
				bool textureExists{false};
				for(int i=0; i<textureCounter; i++){
					if(textureSlots[i] ==  textureId){
						textureIdFloat *= i;
						textureExists = true;
						break;
					}
				}
				if(!textureExists){
					textureIdFloat *= textureCounter;
					textureSlots[textureCounter] = textureId;
					textureCounter += 1;
				}

				for(int i=0; i < vertexPerElementCount; i++){
					vertexBuffer[elementCounter*vertexPerElementCount + i] = vertexStruct[i];
					vertexBuffer[elementCounter*vertexPerElementCount + i].textureIndex = textureIdFloat;
				}
			}else{
				for(int i=0; i < vertexPerElementCount; i++){
					vertexBuffer[elementCounter*vertexPerElementCount + i] = vertexStruct[i];
				}
			}

			elementCounter += 1;
		}

		void setViewTransform(const glm::mat4& viewTransform){
			shader.bind();
			shader.setUniform("viewTransform", viewTransform);
		}

		void flushIfFull(){
			if(isFull())
				flush();
		}

		~BatchRenderer(){
			glDeleteVertexArrays(1, &VAO);
		}

		void setGraphicsStatistics(GraphicsStatistics* graphicsStatisticsPtr){
			graphicsStatistics = graphicsStatisticsPtr;
		}

	private:
		std::array<VertexType, ElementsBufferSize*getVertexCount(renderElement)> vertexBuffer;
		GLuint VAO, VBO, EBO;
		Shader shader;
		GraphicsStatistics* graphicsStatistics{};
		const unsigned int vertexPerElementCount = getVertexCount(renderElement);

		std::size_t textureCounter{0};
		std::size_t elementCounter{0};
		std::array<GLuint, 32> textureSlots{};
};

