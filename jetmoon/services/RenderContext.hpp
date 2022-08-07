#pragma once
#include <GL/glew.h>              // for GLuint
#include <GLFW/glfw3.h>           // for GLFWwindow
#include <memory>
#include <iosfwd>                 // for string
#include <string>                 // for hash, operator==
#include <unordered_map>          // for unordered_map
class Framebuffer;

class RenderContext{
public:
	void init();
	void changeFramebuffer(std::string id);
	void addFramebuffer(std::string id, std::shared_ptr<Framebuffer> fb);
	void destroyFramebuffer(std::string id);
	std::shared_ptr<Framebuffer> getFramebuffer(std::string id);
	~RenderContext();

	GLFWwindow* window; 

	std::shared_ptr<Framebuffer> windowFramebuffer;
	std::shared_ptr<Framebuffer> actualFramebuffer;
	std::unordered_map<std::string, std::shared_ptr<Framebuffer>> framebufferMap;

	bool isVsyncActivated{true};

	static GLuint WindowWidth;
	static GLuint WindowHeight;
	static GLuint ResizedWindow;
};

