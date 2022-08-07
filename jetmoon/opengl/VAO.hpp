#pragma once

#include <initializer_list>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

template <typename VertexType>
class VAO_DS{
	VAO_DS(int elementCount, void* data=nullptr){
	int vertexCount{0};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * vertexCount, nullptr, GL_STATIC_DRAW);

	GLuint* trianglesIndex = new GLuint[elementCount * 6];
	for(int i=0; i < elementCount ; i++) {
		trianglesIndex[i*6 + 0] = 0 + i*4;
		trianglesIndex[i*6 + 1] = 1 + i*4;
		trianglesIndex[i*6 + 2] = 3 + i*4;
		trianglesIndex[i*6 + 3] = 1 + i*4;
		trianglesIndex[i*6 + 4] = 2 + i*4;
		trianglesIndex[i*6 + 5] = 3 + i*4;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementCount * 6, trianglesIndex, GL_STATIC_DRAW);
	delete[] trianglesIndex;

	}

	GLuint VAO, VBO, EBO;
	VertexType* buffer;
};
