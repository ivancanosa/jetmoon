#pragma once

#include "utils/ComponentInspector.hpp"
#include "configShaderDSVariant.h"

struct ProceduralTexture{
	int width{128};
	int height{128};
	bool dynamic{false};
	ShaderDSVariant shader{};
};

COMPONENT_INSPECTOR(ProceduralTexture, width, height, dynamic, shader);
