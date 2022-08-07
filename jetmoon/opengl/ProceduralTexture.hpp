#pragma once

#include "opengl/Framebuffer.hpp"
#include "opengl/ShaderGraphHelper.hpp"
#include "opengl/PostProcessingHelper.hpp"
#include "opengl/Texture.hpp"

class ProceduralTexture{
public:
	ProceduralTexture(){
		auto[a, b, c] = glGenerateUnityVertex();
		VAO = a;
		VBO = b;
		EBO = c;
	}

	std::shared_ptr<Texture> generate(FramebufferDefinition& fbDef, std::string shader, ServiceContext* serviceContext){
		std::shared_ptr<Framebuffer> fb = std::make_shared<Framebuffer>(fbDef);
	}

	std::shared_ptr<Texture> generate(FramebufferDefinition& fbDef, std::vector<std::variant<ShaderDSVariant, ShaderGraph>>& postProcessing, ServiceContext* serviceContext){
	}

	std::shared_ptr<Texture> generate(FramebufferDefinition& fbDef, std::variant<ShaderDSVariant>& postProcessing, ServiceContext* serviceContext){
	}

	~ProceduralTexture(){
		glDeleteVertexArrays(1, &VAO);
	}
private:
	GLuint VAO, VBO, EBO;
};
