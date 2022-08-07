#pragma once

#include "core/ServiceContext.hpp"
#include <vector>

#include <GL/glew.h>

#include "configShaderDSVariant.h"
#include "utils/GLUtils.hpp"
#include "opengl/Framebuffer.hpp"
#include "opengl/Texture.hpp"
#include "core/Time.hpp"

#include "services/ShaderLoader.hpp"


//Assumptions:
//pos -1 is the camera framebuffer
//The framebuffer of the last shader of the vector will be copied to the camera
struct ShaderGraphHelper{
	ShaderGraph* shaderGraph;


	std::shared_ptr<Framebuffer> originalFb;
	std::vector<std::shared_ptr<Framebuffer>> shadersFb{};
	std::vector<bool> generatedShaders;
	ServiceContext* serviceContext;
	int width, height;
	float u_Time{0.};

	void exec(std::shared_ptr<Framebuffer> originalFb, ShaderGraph* shaderGraph, ServiceContext* serviceContext){
		u_Time = Time::fromLaunch/1000000.;
		this->shaderGraph = shaderGraph;
		this->serviceContext = serviceContext;
		this->originalFb = originalFb;
		width = originalFb->width;
		height = originalFb->height;
		if(shadersFb.size() == 0){
			for(int i=0; i<shaderGraph->shaders.size(); i++){
				shadersFb.push_back(nullptr);
			}
		}
		generatedShaders.clear();
		for(int i=0; i<shaderGraph->shaders.size(); i++){
			generatedShaders.push_back(false);
		}
		exec(shaderGraph->shaders.size()-1);
		auto renderToWindowShader = serviceContext->shaderLoader->getShader("renderToWindow");
		renderToWindowShader.bind();
		originalFb->bind();
		shadersFb[shaderGraph->shaders.size()-1]->texturesArray[0]->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void exec(int pos){
		std::array<std::tuple<int, int, int>, 32> shaderConnections;//Prev shader, prev shader tex, actual shader tex
		int countShaderConnections{0};
		int fbTextures{1};
		if(generatedShaders[pos] == true) return;
		for(auto conn: shaderGraph->connections){
			if(conn.shader1 == pos){
				if(conn.shader0 != -1){
					exec(conn.shader0);
				}
				assert(conn.tex0 >= 0 && conn.tex0 < 4 && conn.tex1 >= 0 && conn.tex1 < 32);
				shaderConnections[countShaderConnections] = {conn.shader0, conn.tex0, conn.tex1};
				countShaderConnections += 1;
			}
			if(conn.shader0 == pos && conn.tex0+1 > fbTextures){
				fbTextures = conn.tex0 + 1;
			}
		}
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
				}, shaderGraph->shaders[pos]);
		if(shadersFb[pos] == nullptr){
			FramebufferDefinition fbDef;
			fbDef.width = this->width;
			fbDef.height = this->height;
			fbDef.hasDepth = false;
			for(int i=0; i<fbTextures; i++){
				fbDef.texturesFormat.push_back({GL_RGBA16F, GL_RGBA});
			}
			shadersFb[pos] = std::make_shared<Framebuffer>(fbDef);
		}
		shadersFb[pos]->resize(this->width, this->height);
		shadersFb[pos]->bind();
		for(int i=0; i<countShaderConnections; i++){
			auto[prevShader, tex0, tex1] = shaderConnections[i];
			if(prevShader > -1){
				shadersFb[prevShader]->texturesArray[tex0]->bindAsUnit(tex1);
			}else{
				originalFb->texturesArray[tex0]->bindAsUnit(tex1);
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		generatedShaders[pos] = true;
	}
};

