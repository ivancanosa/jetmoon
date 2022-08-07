#pragma once

#include <GL/glew.h>                     // for GLuint
#include <memory>
#include <array>                         // for array
#include <variant>                       // for variant
#include <vector>                        // for vector
#include "configShaderDSVariant.h"       // for ShaderDSVariant, ShaderGraph...
#include "core/definitions.hpp"          // for Entity, MAX_ENTITIES
#include "opengl/ShaderGraphHelper.hpp"  // for ShaderGraphHelper
class Framebuffer;
struct FramebufferDefinition;
struct ServiceContext;

class PostProcessingHelper{
	public:
		void init(FramebufferDefinition fbDef);

		void exec(Entity entity, std::shared_ptr<Framebuffer> originFb, std::vector<std::variant<ShaderDSVariant, ShaderGraph>>& postVector, ServiceContext* serviceContext);
		~PostProcessingHelper();

	private:
		bool generated{false};
		GLuint VAO, VBO, EBO;
		std::shared_ptr<Framebuffer> framebuffer{};
		std::array<std::vector<ShaderGraphHelper>, MAX_ENTITIES> shaderGraphHelperArray;
};
