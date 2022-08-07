#include "opengl/BatchRenderer.hpp"
#include <GL/glew.h>  // for GLuint, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW


template <>
void generateIndexBuffer<RenderElement::POINT>(GLuint& EBO, std::size_t ElementsBufferSize){
	GLuint* pointsIndex = new GLuint[ElementsBufferSize];
	for(int i=0; i < ElementsBufferSize ; i++) {
		pointsIndex[i] = i;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ElementsBufferSize, pointsIndex, GL_STATIC_DRAW);
	delete[] pointsIndex;
}

template <>
void generateIndexBuffer<RenderElement::LINE>(GLuint& EBO, std::size_t ElementsBufferSize){
	GLuint* linesIndex = new GLuint[ElementsBufferSize * 2];
	for(int i=0; i < ElementsBufferSize ; i++) {
		linesIndex[i*2 + 0] = 0 + i*2;
		linesIndex[i*2 + 1] = 1 + i*2;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ElementsBufferSize * 2, linesIndex, GL_STATIC_DRAW);
	delete[] linesIndex;
}

template <>
void generateIndexBuffer<RenderElement::TRIANGLE>(GLuint& EBO, std::size_t ElementsBufferSize){
	GLuint* trianglesIndex = new GLuint[ElementsBufferSize * 3];
	for(int i=0; i < ElementsBufferSize ; i++) {
		trianglesIndex[i*3 + 0] = 0 + i*3;
		trianglesIndex[i*3 + 1] = 1 + i*3;
		trianglesIndex[i*3 + 2] = 2 + i*3;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ElementsBufferSize * 3, trianglesIndex, GL_STATIC_DRAW);
	delete[] trianglesIndex;
}


template <>
void generateIndexBuffer<RenderElement::SQUAD>(GLuint& EBO, std::size_t ElementsBufferSize){
	GLuint* trianglesIndex = new GLuint[ElementsBufferSize * 6];
	for(int i=0; i < ElementsBufferSize ; i++) {
		trianglesIndex[i*6 + 0] = 0 + i*4;
		trianglesIndex[i*6 + 1] = 1 + i*4;
		trianglesIndex[i*6 + 2] = 3 + i*4;
		trianglesIndex[i*6 + 3] = 1 + i*4;
		trianglesIndex[i*6 + 4] = 2 + i*4;
		trianglesIndex[i*6 + 5] = 3 + i*4;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ElementsBufferSize * 6, trianglesIndex, GL_STATIC_DRAW);
	delete[] trianglesIndex;
}

