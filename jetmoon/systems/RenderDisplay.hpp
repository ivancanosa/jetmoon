#pragma once
#include <string_view>       // for string_view
#include "GL/glew.h"         // for GLuint
#include "core/ISystem.hpp"  // for ISystem
class World;
struct ServiceContext;
struct WorldContext;


class RenderDisplay: public ISystem{
public:
	void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	std::string_view getName() override;
	ISystem* clone() override;

	~RenderDisplay();
private:
	GLuint VAO, VBO, EBO;
};
