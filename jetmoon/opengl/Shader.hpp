#pragma once
#include <GL/glew.h>                    // for GLint, GLsizei, GLenum, glGet...
#include <memory>
#include <stdint.h>                     // for uint32_t
#include <iosfwd>                       // for string
#include <string>                       // for hash, operator==
#include <unordered_map>                // for unordered_map
#include "glm/ext/matrix_float4x4.hpp"  // for mat4
#include "glm/ext/vector_float2.hpp"    // for vec2
#include "glm/ext/vector_float3.hpp"    // for vec3
#include "glm/ext/vector_float4.hpp"    // for vec4
struct Color3;
struct Color4;
struct Vec2;
struct Vec3;
struct Vec4;

class Shader{
public:
	Shader(){}
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath);

	void setUniform(const std::string& name, int value) ;
	void setUniform(const std::string& name, float value) ;
	void setUniform(const std::string& name, const glm::vec2& value) ;
	void setUniform(const std::string& name, const glm::vec3& value) ;
	void setUniform(const std::string& name, const glm::vec4& value) ;
	void setUniform(const std::string& name, const glm::mat4& value) ;

	void setUniform(const std::string& name, const Vec2& value) ;
	void setUniform(const std::string& name, const Vec3& value) ;
	void setUniform(const std::string& name, const Vec4& value) ;

	void setUniform(const std::string& name, const Color3& value) ;
	void setUniform(const std::string& name, const Color4& value) ;


	void setUniform(const std::string& name, int* values, uint32_t count) ;
	void setUniform(const std::string& name, float* values, uint32_t count) ;

	void bind();

	void getUniformsInfo(){
		struct uniform_info { 
			GLint location;
			GLenum type;
			GLsizei count;
		};

		GLint uniform_count = 0;
		glGetProgramiv(shaderId, GL_ACTIVE_UNIFORMS, &uniform_count);

		if (uniform_count != 0) {
			GLint 	max_name_len = 0;
			GLsizei length = 0;
			GLsizei count = 0;
			GLenum 	type = GL_NONE;
			glGetProgramiv(shaderId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

			auto uniform_name = std::make_unique<char[]>(max_name_len);

			std::unordered_map<std::string, uniform_info> uniforms;

			for (GLint i = 0; i < uniform_count; ++i) {
				glGetActiveUniform(shaderId, i, max_name_len, &length, &count, &type, uniform_name.get());

				uniform_info uniform_info = {};
				uniform_info.location = glGetUniformLocation(shaderId, uniform_name.get());
				uniform_info.count = count;
				uniform_info.type = type;

				uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
			}
		}
	}

	GLuint shaderId{0};
};
