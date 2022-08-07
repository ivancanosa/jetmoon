#pragma once

#include "utils/ComponentInspector.hpp"
#include "opengl/Framebuffer.hpp"

#include "configShaderDSVariant.h"

struct CameraComponent{
	bool active{false};
	float scale{1.0};
	std::vector<std::variant<ShaderDSVariant, ShaderGraph>> postProcessingPipeline{};

	std::shared_ptr<Framebuffer> framebuffer{nullptr};
};

COMPONENT_INSPECTOR(CameraComponent, active, scale, postProcessingPipeline);
