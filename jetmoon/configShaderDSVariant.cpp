#include "configShaderDSVariant.h"
#include <type_traits>  // for remove_extent_t

void Bloom::generate() {
	shaderGraph = std::make_shared<ShaderGraph>();
	shaderGraph->shaders.emplace_back(Bloom0());
	shaderGraph->shaders.emplace_back(Bloom1());
	shaderGraph->shaders.emplace_back(AddTexture());
	shaderGraph->connections.push_back({-1, 0, 0, 0});
	shaderGraph->connections.push_back({0, 1, 0, 0});
	shaderGraph->connections.push_back({1, 2, 0, 1});
	shaderGraph->connections.push_back({-1, 2, 0, 0});
}

void Bloom::overrideFields(){
	std::get<Bloom0>(shaderGraph->shaders[0]).threshold = threshold;
	std::get<AddTexture>(shaderGraph->shaders[2]).intensity = intensity;
}
