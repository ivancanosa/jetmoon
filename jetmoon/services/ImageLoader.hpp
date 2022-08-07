#pragma once

#include <filesystem>
#include <iostream>
#include <assert.h>
#include <algorithm>

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils/stb_image.hpp"

class ImageLoader{
	public:

		ImageLoader(){
			stbi_set_flip_vertically_on_load(true);  
			nameToPathMap.insert({"missingTexture", "data/errorAssets/missingTexture.png"});
		}

		~ImageLoader(){
			this->clear();
			nameToPathMap.clear();
		}


		void addDir(std::string dir){
			pathsVector.push_back(dir);
			internalAddDir(dir);
		}

		void hotReload(){
			for(auto id: texturesIdVector){
				glDeleteTextures(1, &id);
			}
			texturesIdVector.clear();
			textureSize.clear();
			nameToPathMap.clear();
			nameToId.clear();
			nameToPathMap.insert({"missingTexture", "data/errorAssets/missingTexture.png"});
			for(auto& path: pathsVector){
				internalAddDir(path);
			}
		}

		void addImage(std::string imagePathStr){
			namespace fs = std::filesystem;
			const fs::path imagePath;
			std::string filename = imagePath.stem().string();
			nameToPathMap.insert( {filename, imagePath.string()} );
		}

		void setDefaultImage(std::string imageId){
			defaultImage = imageId;
		}

		GLuint load(std::string imageId){
			GLuint id{};
			if(nameToId.find(imageId) == nameToId.end()){
				if(nameToPathMap.find(imageId) == nameToPathMap.end()){
					imageId = "missingTexture";
				}
				//assert((nameToPathMap.find(imageId) != nameToPathMap.end()) && "The image are not registered");
				id = this->loadImage(nameToPathMap[imageId]);
				texturesIdVector.emplace_back(id);
				nameToId.insert( {imageId, id} );
				return id;
			}
			return nameToId[imageId];
		}

		std::tuple<GLuint, int, int> loadWithSize(std::string imageId){
			auto id = this->load(imageId);
			auto[width, height] = this->textureSize[id];
			return {id, width, height};
		}

		bool isLoaded(std::string imageId){
			return nameToPathMap.find(imageId) != nameToPathMap.end();
		}

		void unload(std::string imageId){
			if(nameToId.find(imageId) != nameToId.end()){
				glDeleteTextures(1, &nameToId[imageId]);
				texturesIdVector.erase(std::remove(texturesIdVector.begin(), texturesIdVector.end(), nameToId[imageId]), texturesIdVector.end());
				nameToId.erase(nameToId.find(imageId));
			}
		}

		void clear(){
			glDeleteTextures(texturesIdVector.size(), &texturesIdVector[0]);
			texturesIdVector.clear();
			nameToId.clear();
		}

	private:

		void internalAddDir(std::string dir){
			namespace fs = std::filesystem;
			assert(fs::is_directory(dir) && "Directory does not exists");

			const fs::path pathToShow{dir};

			for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
					 iterEntry != fs::recursive_directory_iterator(); 
					 ++iterEntry ) {
				const auto filenameStr = iterEntry->path().stem().string();
				if(!iterEntry->is_directory()){
					assert((nameToPathMap.find(filenameStr) == nameToPathMap.end()) && "Image id duplicate");
					nameToPathMap.insert( {filenameStr, iterEntry->path().string()} );
				}
			}

		}

		GLuint loadImage(std::string path, GLuint mipmapLevel=0){
			GLuint textureId;
			glGenTextures(1, &textureId);  
			glBindTexture(GL_TEXTURE_2D, textureId);  


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			int width, height, nrChannels;
			unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0); 
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			if(data){
				if(nrChannels == 3){
					glTexImage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				}else if(nrChannels == 4){
					glTexImage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}else{
					assert(false && "Image hasn't 3 or 4 channels");
				}

				glGenerateMipmap(GL_TEXTURE_2D);
			}else{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
			this->textureSize.insert( {textureId, {width, height}} );
			return textureId;
		}


		std::vector<GLuint> texturesIdVector{};
		std::unordered_map<std::string, std::string> nameToPathMap{};
		std::unordered_map<std::string, GLuint> nameToId{};
		std::unordered_map<GLuint, std::tuple<int, int>> textureSize{};
		std::string defaultImage{""};
		std::vector<std::string> pathsVector{};
};
