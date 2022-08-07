#include "opengl/Shader.hpp"

#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/definitions.hpp"

#include "utils/GLUtils.hpp"

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath){
	if(geometryShaderPath == ""){
		shaderId = loadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	}else{
		shaderId = loadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), geometryShaderPath.c_str());
	}
}

void Shader::setUniform(const std::string& name, int value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform1i(loc, value);
}

void Shader::setUniform(const std::string& name, float value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform1f(loc, value);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform2f(loc, value[0], value[1]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform3f(loc, value[0], value[1], value[2]);
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform4f(loc, value[0], value[1], value[2], value[3]);
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}


void Shader::setUniform(const std::string& name, const Vec2& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform2f(loc, value.x, value.y);
}

void Shader::setUniform(const std::string& name, const Vec3& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& name, const Vec4& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const std::string& name, const Color3& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& name, const Color4& value) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}




void Shader::setUniform(const std::string& name, int* values, uint32_t count) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform1iv(loc, count, values);
}

void Shader::setUniform(const std::string& name, float* values, uint32_t count) {
	GLuint loc = glGetUniformLocation(shaderId, name.c_str());
	glUniform1fv(loc, count, values);
}

void Shader::bind(){
	glUseProgram(shaderId);
}

