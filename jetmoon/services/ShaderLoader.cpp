#include "ShaderLoader.hpp"
#include <GL/glew.h>                         // for glDeleteShader
#include <filesystem>                        // for path
#include <fstream>                           // for string, basic_istream
#include <initializer_list>                  // for initializer_list
#include <string>                            // for char_traits, hash, opera...
#include <unordered_map>                     // for unordered_map, __hash_ma...
#include "opengl/Shader.hpp"                 // for Shader
#include "utils/json.hpp"                    // for basic_json<>::object_t

void ShaderLoader::init(const std::string& shadersDefinitionPath){
	this->shadersDefinitionPath = shadersDefinitionPath;
	loadShaders();
}

void ShaderLoader::hotReload(){
	for(auto&[_, shader]: shaderMapping){
		glDeleteShader(shader.shaderId);
	}
	shaderMapping.clear();
	for(auto&[id, vertex, fragment, geometry]: shadersDefinitionVector){
		shaderMapping.insert({ id, {vertex, fragment, geometry} });
	}
	loadShaders();
}

void ShaderLoader::loadShaders(){
	std::filesystem::path file{shadersDefinitionPath};
	std::ifstream t(file.string());
	std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	auto j = nlohmann::json::parse(jsonStr);
	shadersDefinitionVector = j.get<std::vector<ShaderDefinition>>();
	for(auto& shaderDef: shadersDefinitionVector){
		shaderMapping.insert({ shaderDef.id, {shaderDef.vertexPath, shaderDef.fragmentPath, shaderDef.geometryPath} });
	}

}

Shader ShaderLoader::getShader(std::string identifier){
	return shaderMapping[identifier];
}
