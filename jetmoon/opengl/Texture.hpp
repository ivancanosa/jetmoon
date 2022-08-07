#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

struct Texture{
	GLuint width, height, internalFormat, format;
	GLuint textureId{0};

	void generate(){
		if(textureId != 0){
			glDeleteTextures(1, &textureId);
		}
		glGenTextures(1, &textureId);  
		glBindTexture(GL_TEXTURE_2D, textureId);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
	}

	std::shared_ptr<Texture> clone(){
			auto texture = std::make_shared<Texture>();
			texture->width = width;
			texture->height = height;
			texture->internalFormat = internalFormat;
			texture->format = format;

			glGenTextures(1, &texture->textureId);  
			glBindTexture(GL_TEXTURE_2D, texture->textureId);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, texture->width, texture->height, 0, texture->format, GL_UNSIGNED_BYTE, 0);
			return texture;
	}

	void resize(int width, int height){
		if(this->width != width || this->height != height){
			this->width = width;
			this->height = height;
			glDeleteTextures(1, &textureId);
			glGenTextures(1, &textureId);  
			glBindTexture(GL_TEXTURE_2D, textureId);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
		}
	}

	void copy(std::shared_ptr<Texture> texture){
		glCopyImageSubData(texture->textureId, GL_TEXTURE_2D, 0, 0, 0, 0,
						   textureId,          GL_TEXTURE_2D, 0, 0, 0, 0,
						   width, height, 1);
	}

	void bind(){
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void bindAsUnit(int pos=0){
		glBindTextureUnit(pos, textureId);
	}

	~Texture(){
		glDeleteTextures(1, &textureId);
	}
};
