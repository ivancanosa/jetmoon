#pragma once

#include <unordered_map>
#include "commonStructures/AnimationDefinition.hpp"

class AnimationDefinitionService{
public:
	AnimationDefinitionService(){
		animationDefinitionSet.insert({"missingTexture", {}});
		defaultAnimation = "missingTexture";
	}

	void loadDir(std::string dir){
		pathsVector.push_back(dir);
		animationDefinitionSet = loadVectorJson<AnimationDefinition>(std::move(dir), [](AnimationDefinition a, std::string path){return path;});
	}

	void hotReload(){
		animationDefinitionSet.clear();
		animationDefinitionSet.insert({"missingTexture", {}});
		defaultAnimation = "missingTexture";
		for(auto& dir: pathsVector){
			animationDefinitionSet = loadVectorJson<AnimationDefinition>(dir, [](AnimationDefinition a, std::string path){return path;});
		}
	}

	void saveToPath(std::string path, std::string id){

	}

	void removeAnimation(std::string id){
		animationDefinitionSet.erase(id);
	}

	bool exists(std::string id){
		return animationDefinitionSet.find(id) != animationDefinitionSet.end();
	}

	void loadAnimation(std::string id, AnimationDefinition animation){
		animationDefinitionSet[id] = animation;
	}

	AnimationDefinition& getAnimationDefinition(std::string id){
		if(animationDefinitionSet.find(id) == animationDefinitionSet.end()){
			return animationDefinitionSet[defaultAnimation];
		}
		return animationDefinitionSet[id];
	}

	std::unordered_map<std::string, AnimationDefinition> animationDefinitionSet{};
	std::string defaultAnimation{};
	std::vector<std::string> pathsVector{};
};
