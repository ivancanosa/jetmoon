#pragma once

#include <GL/glew.h>              // for GLuint
#include <memory>
#include <array>                  // for array
#include <tuple>                  // for tuple
#include <vector>                 // for vector
struct Texture;

struct FramebufferDefinition{
	GLuint width, height;
	bool hasDepth;
	std::vector<std::tuple<GLuint, GLuint>> texturesFormat;
};

class Framebuffer{
public:
	Framebuffer(FramebufferDefinition fbDef);

	//This is only for the window framebuffer
	Framebuffer(GLuint id, GLuint width, GLuint height, bool hasDepth);

	void generateTexture(GLuint index, GLuint internalFormat, GLuint format);

	void resize(int width, int height);

	void generateDepth();

	void bind();

	void unBind();

	~Framebuffer();

	GLuint width, height, hasDepth;
	GLuint id;
	GLuint attachmentsCount;
	unsigned int rbo;
	std::array<std::shared_ptr<Texture>, 4> texturesArray;
	FramebufferDefinition fbDef;

private:
	void loadFramebufferDefinition();

	bool isWindowFramebuffer{false};
};
