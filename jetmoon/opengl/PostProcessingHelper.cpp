#include "PostProcessingHelper.hpp"
#include <type_traits>                // for remove_extent_t, decay_t, is_ba...
#include "GL/glew.h"                  // for glClear, glDrawElements, GL_COL...
#include "core/ServiceContext.hpp"    // for ServiceContext
#include "core/Time.hpp"              // for Time, Time::fromLaunch
#include "opengl/Framebuffer.hpp"     // for Framebuffer
#include "opengl/Texture.hpp"     
#include "opengl/Shader.hpp"          // for Shader
#include "services/ShaderLoader.hpp"  // for ShaderLoader
#include "utils/GLUtils.hpp"          // for glGenerateUnityVertex
#include "utils/visit_struct.hpp"     // for for_each

void PostProcessingHelper::exec(Entity entity, std::shared_ptr<Framebuffer> originFb, std::vector<std::variant<ShaderDSVariant, ShaderGraph>>& postVector, ServiceContext* serviceContext){
	bool renderToAux = true;
	auto& shaderGraphHelperVector = shaderGraphHelperArray[entity];
	int i = 0;
	if(!generated){
		auto[a, b, c] = glGenerateUnityVertex();
		VAO = a;
		VBO = b;
		EBO = c;
		framebuffer = std::make_shared<Framebuffer>(originFb->fbDef);
		generated = true;
	}
	framebuffer->resize(originFb->width, originFb->height);
	glBindVertexArray(VAO);
	float u_Time = Time::fromLaunch/1000000.;
	for(auto& shaderVariant: postVector){
		std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, ShaderDSVariant>){
				std::visit([&](auto&& arg){
						if constexpr(!std::is_base_of<TypedShaderGraph,typename std::remove_reference<decltype(arg)>::type>()){
							auto shader = serviceContext->shaderLoader->getShader(arg.shader);
							shader.bind();
							shader.setUniform("u_Time", u_Time);
							visit_struct::for_each(arg,
									[&](const char * name, const auto & value) {
									shader.setUniform(name, value);	
									});
							}
						else{
							if(shaderGraphHelperVector.size() <= i){
								shaderGraphHelperVector.push_back({});
							}
							auto& shaderGraphHelper = shaderGraphHelperVector[i];
							if(renderToAux){
								shaderGraphHelper.exec(originFb, arg.getShaderGraph(), serviceContext);
							}else{
								shaderGraphHelper.exec(framebuffer, arg.getShaderGraph(), serviceContext);
							}
							i++;
						}
						}, arg);
				if(renderToAux){
					framebuffer->bind();
					originFb->texturesArray[0]->bind();
				}else{
					originFb->bind();
					framebuffer->texturesArray[0]->bind();
				}
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderToAux = !renderToAux;
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}else if constexpr(std::is_same_v<T, ShaderGraph>){
				if(shaderGraphHelperVector.size() <= i){
					shaderGraphHelperVector.push_back({});
				}
				auto& shaderGraphHelper = shaderGraphHelperVector[i];
				if(renderToAux){
					shaderGraphHelper.exec(originFb, &arg, serviceContext);
				}else{
					shaderGraphHelper.exec(framebuffer, &arg, serviceContext);
				}
				i++;
			}
		}, shaderVariant);
	}
	if(!renderToAux){
		auto renderToWindowShader = serviceContext->shaderLoader->getShader("renderToWindow");
		renderToWindowShader.bind();
		originFb->bind();
		framebuffer->texturesArray[0]->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

PostProcessingHelper::~PostProcessingHelper(){
	glDeleteVertexArrays(1, &VAO);
	framebuffer = nullptr;
}
