#include "RenderClear.hpp"
#include <GL/glew.h>                       // for glClear, glClearColor, GL_...
#include <memory>
#include <iosfwd>                          // for string
#include <type_traits>                     // for remove_extent_t
#include "commonStructures/Camera.hpp"     // for Camera
#include "commonStructures/ConfigDS.hpp"    
#include "components/CameraComponent.hpp"  // for CameraComponent
#include "core/ServiceContext.hpp"         // for ServiceContext, EditorDS
#include "core/World.hpp"                  // for World
#include "core/WorldContext.hpp"           // for WorldContext
#include "core/definitions.hpp"            // for Entity, NullEntity
#include "services/RenderContext.hpp"      // for RenderContext, RenderConte...
#include "services/Renderer2D.hpp"         // for Renderer2D
#include "services/EditorDS.hpp"         
class Framebuffer;

#include <iostream>

std::string_view RenderClear::getName(){
	static std::string str{"RenderClear"};
	return str;
}

ISystem* RenderClear::clone(){
	return new RenderClear();
}

void RenderClear::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
	//auto camera = &worldContext->camera;
	//GLint viewport[4]{};
	//glGetIntegerv(GL_VIEWPORT,viewport);
	//camera->scale.x /= float(viewport[2])/float(viewport[3]);
	anteriorWidth = serviceContext->configDS->graphicsOptions.resolutionWidth;
	anteriorHeight = serviceContext->configDS->graphicsOptions.resolutionHeight;
	anteriorScreenMode = ScreenMode::None;

//	changeWindowMode(serviceContext);
}

void RenderClear::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
#ifdef __RELEASE__
	changeWindowMode(serviceContext);
#endif

	std::shared_ptr<Framebuffer> fb = serviceContext->renderContext->actualFramebuffer;
	auto camera = &worldContext->camera;

	camera->ratioX = float(fb->height)/float(fb->width);
	if(RenderContext::ResizedWindow){
		RenderContext::ResizedWindow = false;
		auto& renderContext = serviceContext->renderContext;
		renderContext->windowFramebuffer->width = RenderContext::WindowWidth;
		renderContext->windowFramebuffer->height = RenderContext::WindowHeight;
	}
	Entity activeCamera = worldContext->camera.activeCamera;
	if(serviceContext->editorDS->isPlaying && activeCamera != NullEntity){
		auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
		cameraComponent.framebuffer->bind();
		camera->ratioX = (float)cameraComponent.framebuffer->height / cameraComponent.framebuffer->width;
	}else{

#ifdef __RELEASE__
		if(activeCamera != NullEntity){
			auto& cameraComponent = world->getComponent<CameraComponent>(activeCamera);
			cameraComponent.framebuffer->bind();
			camera->ratioX = (float)cameraComponent.framebuffer->height / cameraComponent.framebuffer->width;
		}else{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);  
		}
#endif

#ifndef __RELEASE__
		serviceContext->renderContext->changeFramebuffer("editor");
#endif
	}

	glClearColor(0., 0., 0., 1.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldContext->camera.updateViewTransform();
	auto view = camera->getTransformationMatrix();
	serviceContext->renderer2D->setCameraTransform(view);

}

void RenderClear::changeWindowMode(ServiceContext* serviceContext){
	int width = serviceContext->configDS->graphicsOptions.resolutionWidth;
	int height = serviceContext->configDS->graphicsOptions.resolutionHeight;
	ScreenMode screenMode = serviceContext->configDS->graphicsOptions.screenMode;
	if(width != RenderContext::WindowWidth || height != RenderContext::WindowHeight || anteriorScreenMode != screenMode){
		auto* window = serviceContext->renderContext->window;
		switch(screenMode){
			case ScreenMode::Windowed:
				anteriorWidth = width;
				anteriorHeight = height;
				int posX, posY;
				glfwGetWindowPos(window, &posX, &posY);
				glfwSetWindowMonitor(window, NULL, posX, posY, width, height, 0);
				RenderContext::WindowWidth = width;
				RenderContext::WindowHeight = height;
				RenderContext::ResizedWindow = true;
				break;
			case ScreenMode::Fullscreen:
				{
				GLFWmonitor* monitor = glfwGetWindowMonitor(window);
				if(monitor == NULL){
					monitor = glfwGetPrimaryMonitor();
				}
				const GLFWvidmode * mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, width, height, mode->refreshRate);
				RenderContext::WindowWidth = mode->width;
				RenderContext::WindowHeight = mode->height;
				RenderContext::ResizedWindow = true;
				}
				break;
			case ScreenMode::Borderless:
				{
				GLFWmonitor* monitor = glfwGetWindowMonitor(window);
				if(monitor == NULL){
					monitor = glfwGetPrimaryMonitor();
				}
				const GLFWvidmode * mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
				RenderContext::WindowWidth = mode->width;
				RenderContext::WindowHeight = mode->height;
				RenderContext::ResizedWindow = true;
				}
				break;
		}
	}
	anteriorScreenMode = screenMode;
}
