#include "opengl/Framebuffer.hpp"
#include <GL/glew.h>           // for GL_FRAMEBUFFER, GLuint, glTexParameteri
#include <assert.h>            // for assert
#include <stddef.h>            // for NULL
#include <array>               // for array, array<>::value_type
#include <type_traits>         // for remove_extent_t
#include "opengl/Texture.hpp"  // for Texture

Framebuffer::Framebuffer(FramebufferDefinition fbDef) {
	this->fbDef = fbDef;
	loadFramebufferDefinition();
}

void Framebuffer::resize(int width, int height){
	if(width == this->width && height == this->height) return;
	glDeleteFramebuffers(1, &id);
	attachmentsCount = 0;
	fbDef.width = width;
	fbDef.height = height;
	if(hasDepth){
		glDeleteRenderbuffers(1, &rbo);
	}
	loadFramebufferDefinition();
}

void Framebuffer::loadFramebufferDefinition(){
	this->width = fbDef.width;
	this->height = fbDef.height;
	this->hasDepth = fbDef.hasDepth;
	assert(width > 0 && height > 0 && "Width or Height invalid for the framebuffer");
	GLint fbAux;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbAux);
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);  

	attachmentsCount = 0;
	for(auto[internalFormat, format]: fbDef.texturesFormat){
		generateTexture(attachmentsCount, internalFormat, format);
		attachmentsCount += 1;
	}
	if(hasDepth){
		generateDepth();
		glEnable (GL_DEPTH_TEST);
	}

	GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(attachmentsCount, buffers);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Error");
	glBindFramebuffer(GL_FRAMEBUFFER, fbAux);  
}

Framebuffer::Framebuffer(GLuint id, GLuint width, GLuint height, bool hasDepth):
	width(width),
	height(height),
	hasDepth(hasDepth),
	id(id),
	isWindowFramebuffer(true)
{}


void Framebuffer::generateTexture(GLuint index, GLuint internalFormat, GLuint format){
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texColorBuffer, 0);
	texturesArray[index] = std::make_shared<Texture>();
	texturesArray[index]->textureId = texColorBuffer;
	texturesArray[index]->width = width;
	texturesArray[index]->height = height;
	texturesArray[index]->internalFormat = internalFormat;
	texturesArray[index]->format = format;
}

void Framebuffer::generateDepth(){
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8 , width, height);  
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(false && "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}

void Framebuffer::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);  
	glViewport(0, 0, width, height);
}

void Framebuffer::unBind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

Framebuffer::~Framebuffer(){
	if(!isWindowFramebuffer){
		glDeleteFramebuffers(1, &id);
		if(hasDepth){
			glDeleteRenderbuffers(1, &rbo);
		}
	}
}
