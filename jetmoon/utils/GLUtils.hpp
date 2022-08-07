#pragma once

#include <GL/glew.h>                  // for GLuint
#include <iosfwd>                     // for string
#include <string>                     // for hash
#include <tuple>                      // for tuple
#include <unordered_map>              // for unordered_map
#include "glm/ext/vector_float4.hpp"  // for vec4

std::tuple<GLuint,GLuint> glGenerateTexture(int width, int height, bool genDepht = false);
std::tuple<GLuint,GLuint,GLuint> glGenerateTexture2(int width, int height, bool genDepht = false);

GLuint generateMonocolorTexture(glm::vec4 color);

//This returns VBO, VAO EBO
std::tuple<GLuint, GLuint, GLuint> glGenerateSprite(float xo=0.f, float yo=0.f);
std::tuple<GLuint, GLuint, GLuint> glGenerateUnityVertex();

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path, const char* geometry_file_path=nullptr);
GLuint loadComputeShader(const char * computeShaderPath);

//TODO: the vertical atlas
struct VerticalAtlas{
	GLuint imageID{0};
	GLuint imagesCount{0};
	std::unordered_map<std::string, GLuint> imagesMap{};
};
