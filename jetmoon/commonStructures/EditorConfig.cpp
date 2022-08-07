#include "EditorConfig.hpp"
#include <exception>                         // for exception
#include <filesystem>                        // for exists, path
#include <initializer_list>                  // for initializer_list
#include <streambuf>                         // for basic_streambuf
											 
void EditorConfig::load(){
	if(std::filesystem::exists(editorConfigPath)){
		try{
			std::filesystem::path file(editorConfigPath);
			std::ifstream t(file.string());
			std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			auto j = nlohmann::json::parse(jsonStr);
			*this = j.get<EditorConfig>();
		}catch(std::exception e){
		}
	}
}

void EditorConfig::save(){
	nlohmann::json j(*this);
	std::string data = j.dump(2);
	std::ofstream out(editorConfigPath);
	out << data;
	out.close();
}
