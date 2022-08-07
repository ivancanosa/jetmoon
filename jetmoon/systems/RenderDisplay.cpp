#include "RenderDisplay.hpp"
#include <GL/glew.h>                      // for glDeleteVertexArrays
#include <iosfwd>                         // for string
#include "GLFW/glfw3.h"                   // for glfwSwapInterval, glfwSwapB...
#include "commonStructures/ConfigDS.hpp"  // for ConfigDS, GraphicsConfig
#include "services/ShaderLoader.hpp"
#include "opengl/Shader.hpp"
#include "opengl/Framebuffer.hpp"
#include "opengl/Texture.hpp"
#include "services/RenderContext.hpp"     // for RenderContext
#include "utils/GLUtils.hpp"              // for glGenerateUnityVertex
#include "core/ServiceContext.hpp"        // for ServiceContext
#include "core/World.hpp"
#include "core/WorldContext.hpp"
#include "components/CameraComponent.hpp"
#include "imgui/imgui_impl_glfw.h"   

#include <iostream>

void RenderDisplay::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	auto[a, b, c] = glGenerateUnityVertex();
	VAO = a;
	VBO = b;
	EBO = c;
}

void RenderDisplay::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
#ifdef __RELEASE__
	Entity activeCamera = worldContext->camera.activeCamera;
	if(activeCamera != NullEntity){
		serviceContext->renderContext->changeFramebuffer("window");
		auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
		auto renderToWindowShader = serviceContext->shaderLoader->getShader("renderToWindow");

		struct Vertex{
			Vec3 vert;
			Vec2 tex;
		};
		Vertex vertex[4] = {
			{{1.f,  1.f, 0.0f},	 {1.f, 1.f}},
			{{1.f,  -1.f, 0.0f}, {1.f, 0.f}},
			{{-1.f,  -1.f, 0.0f},{0.f, 0.f}},
			{{-1.f,  1.f, 0.0f}, {0.f, 1.f}}
		};


		Vec2 wsize;
		wsize.x = serviceContext->renderContext->windowFramebuffer->width;
		wsize.y = serviceContext->renderContext->windowFramebuffer->height;
		Vec2 viewportSize;
		viewportSize.x = cameraComponent.framebuffer->texturesArray[0]->width;
		viewportSize.y = cameraComponent.framebuffer->texturesArray[0]->height;
		Vec2 padding = {0, 0};
		float ratioOrigin = (float)viewportSize.x / viewportSize.y;
		float ratioDestiny = wsize.x / wsize.y;
		float w, h;
		if(ratioOrigin >= ratioDestiny){
			w = wsize.x;
			h = wsize.y * (ratioDestiny/ratioOrigin);
			padding.y = (wsize.y - h)/2;
		}else{
			h = wsize.y;
			w = wsize.x * (ratioOrigin/ratioDestiny);
			padding.x = (wsize.x - w)/2;
		}
		padding.x = (padding.x*2) / wsize.x;
		padding.y = (padding.y*2) / wsize.y;

		vertex[0].vert = {(float)1.-padding.x, (float)1.-padding.y, 0.};
		vertex[1].vert = {(float)1.-padding.x, (float)-1.+padding.y, 0.};
		vertex[2].vert = {(float)-1.+padding.x, (float)-1.+padding.y, 0.};
		vertex[3].vert = {(float)-1.+padding.x, (float)1.-padding.y, 0.};

		/*nuklear_changedViewport = 1;
		nuklear_viewport[0] = wsize.x*padding.x*0.5;
		nuklear_viewport[1] = wsize.y*padding.y*0.5;
		nuklear_viewport[2] = viewportSize.x / w;
		nuklear_viewport[3] = viewportSize.y / h;*/

		imgui_changedViewport = 1;
		imgui_viewport[0] = wsize.x*padding.x*0.5;
		imgui_viewport[1] = wsize.y*padding.y*0.5;
		imgui_viewport[2] = viewportSize.x / w;
		imgui_viewport[3] = viewportSize.y / h;

		serviceContext->renderContext->windowFramebuffer->bind();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
		renderToWindowShader.bind();
		cameraComponent.framebuffer->texturesArray[0]->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}else{
		//Render nothing to window
	}
#endif
	if(serviceContext->configDS->graphicsOptions.vsync && !serviceContext->renderContext->isVsyncActivated){
		glfwSwapInterval(1); // Enable vsync
		serviceContext->renderContext->isVsyncActivated = true;
	}else if (!serviceContext->configDS->graphicsOptions.vsync && serviceContext->renderContext->isVsyncActivated){
		glfwSwapInterval(0); // Disable vsync
		serviceContext->renderContext->isVsyncActivated = false;
	}
	glfwSwapBuffers(serviceContext->renderContext->window);
}


std::string_view RenderDisplay::getName(){
	static std::string str{"RenderDisplay"};
	return str;
}

ISystem* RenderDisplay::clone(){
	return new RenderDisplay();
}
RenderDisplay::~RenderDisplay(){
	glDeleteVertexArrays(1, &VAO);
}
