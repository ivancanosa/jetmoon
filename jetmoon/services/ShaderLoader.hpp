#pragma once
#include <iosfwd>             // for string
#include <string>             // for hash, basic_string, operator==
#include <unordered_map>      // for unordered_map
#include <vector>             // for vector
#include "opengl/Shader.hpp"  // for Shader
#include "utils/json.hpp"     // for NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH...

struct ShaderDefinition{
	ShaderDefinition(){}
	std::string id, vertexPath, fragmentPath, geometryPath{""};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ShaderDefinition, id, vertexPath, fragmentPath, geometryPath);


class ShaderLoader{
public:
	void init(const std::string& shadersDefinitionPath);
	void hotReload();
	Shader getShader(std::string identifier);

private:
	void loadShaders();

	std::vector<ShaderDefinition> shadersDefinitionVector{};
	std::unordered_map<std::string, Shader> shaderMapping;
	std::string shadersDefinitionPath;
};
