#include "RenderContext.hpp"
#include <GL/glew.h>               // for glEnable, GLuint, glBlendFunc, glG...
#include <GLFW/glfw3.h>            // for glfwWindowHint, glfwSetInputMode
#include <tuple>
#include <assert.h>                // for assert
#include <stdio.h>                 // for fclose, fputs, fopen, fread, fseek
#include <cstdlib>                 // for exit, free, NULL, calloc
#include <iostream>                // for string, operator<<, endl, basic_os...
#include <type_traits>             // for remove_extent_t
#include "opengl/Framebuffer.hpp"  // for Framebuffer

GLuint RenderContext::WindowWidth = 0;
GLuint RenderContext::WindowHeight = 0;
GLuint RenderContext::ResizedWindow = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	RenderContext::WindowWidth = width;
	RenderContext::WindowHeight = height;
	RenderContext::ResizedWindow = true;

//    glViewport(0, 0, width, height);
/*	auto camera = Locator<Camera>::getService();
	camera->ratioX = float(height)/float(width);*/
}

void RenderContext::init(){
	glewExperimental = true; // Needed for core profile
	if( !glfwInit() ) {
		std::cerr << "Failed to initialize GLWM" << std::endl;
		std::exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
//	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // No resizable

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(RenderContext::WindowWidth, RenderContext::WindowHeight, "Hello World", NULL, NULL);
	if( window == NULL ){
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
    glfwSwapInterval(1); // Enable vsync
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		std::cerr <<  "Failed to initialize GLEW" << std::endl;
		std::exit(-1);
	}

	const char* gamepadDatabase = "data/.gamecontrollerdb.txt";
	FILE *fp;
	long lSize;
	char *mappings;

	fp = fopen ( gamepadDatabase, "rb" );
	if( !fp ) perror(gamepadDatabase),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	/* allocate memory for entire content */
	mappings = (char*)calloc( 1, lSize+1 );
	if( !mappings ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

	/* copy the file into the mappings */
	if( 1!=fread( mappings , lSize, 1 , fp) )
	  fclose(fp),free(mappings),fputs("entire read fails",stderr),exit(1);

	glfwUpdateGamepadMappings(mappings);
	fclose(fp);
	free(mappings);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); 


	//glDisable(GL_DEPTH_TEST); 
//	glDepthFunc( GL_ALWAYS);
	//glDepthMask(GL_FALSE);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	GLint f;
	GLuint ff;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &f);
	ff = f;

	std::shared_ptr<Framebuffer> windowFramebuffer = std::make_shared<Framebuffer>(ff, RenderContext::WindowWidth, RenderContext::WindowHeight, true);
	framebufferMap.insert( {"window", windowFramebuffer} );
	this->windowFramebuffer = windowFramebuffer;
	this->actualFramebuffer = windowFramebuffer;
}

void RenderContext::changeFramebuffer(std::string id){
	for(auto[fbStr, fbPtr]: framebufferMap){
		if(fbStr == id){
			actualFramebuffer = fbPtr;
			if(id == "window"){
				fbPtr->width = RenderContext::WindowWidth;
				fbPtr->height = RenderContext::WindowHeight;
			}
			fbPtr->bind();
			return;
		}
	}
}

void RenderContext::addFramebuffer(std::string id, std::shared_ptr<Framebuffer> fb){
	framebufferMap[id] = fb;
}

void RenderContext::destroyFramebuffer(std::string id){
	for(auto[fbStr, fbPtr]: framebufferMap){
		if(fbStr == id){
			if(actualFramebuffer->id == fbPtr->id){
				actualFramebuffer->unBind();
			}
			framebufferMap.erase(id);
			return;
		}
	}
}




std::shared_ptr<Framebuffer> RenderContext::getFramebuffer(std::string id){
	for(auto[fbStr, fbPtr]: framebufferMap){
		if(fbStr == id){
			return fbPtr;
		}
	}
	assert(false && "Attempted to get a non existent Framebuffer");
	return nullptr;
}

RenderContext::~RenderContext(){
	windowFramebuffer = nullptr;
	actualFramebuffer = nullptr;
	framebufferMap.clear();
}
