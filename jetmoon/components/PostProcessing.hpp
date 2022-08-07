#pragma once
#include "utils/ComponentInspector.hpp"
#include "configShaderDSVariant.h"

struct PostProcessing{
	std::vector<std::variant<ShaderDSVariant, ShaderGraph>> postProcessingPipeline{};
};

COMPONENT_INSPECTOR(PostProcessing, postProcessingPipeline);
