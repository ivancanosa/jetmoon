#pragma once

#include <string>
#include <unordered_map>
#include <tuple>

//TODO change the unordered_map to one which is stacically allocated
struct Animation{
	int margin, padding, frameWidth, frameHeight;
	float frameDuration;
	std::unordered_map<std::string, std::tuple<int, int, bool>> animations;
};

struct AnimationList{
	std::unordered_map<std::string, Animation> animations;
};

struct AnimationComponent{
	std::string spriteSheetName;
	std::string animationName;
	std::string actualAnimation;
	float actualDelta;
};

class AnimatorController{
	public:
		AnimatorController(int frameWidthArg, int frameHeightArg, float frameDurationArg, int marginArg=0, int paddingArg=0):
			frameWidth(frameWidthArg), frameHeight(frameHeightArg),
			margin(marginArg), padding(paddingArg),
			frameDuration(frameDurationArg), actualDelta(0.), actualAnimation(""){}

		void addAnimation(std::string name, int row, int maxFrames, bool loop=true){
			animations.insert({name, {row, maxFrames, loop}});
		}

		//Return {posX, posY, frameWidth, frameHeight}
		std::tuple<int, int, int, int> getFrame(float deltaTime){
			actualDelta += deltaTime;
			auto[iRow, iCollumn] = actualFrame;
			if(actualDelta >= frameDuration){
				actualDelta -= frameDuration;
				auto[_1, maxFrames, loop] = animations[actualAnimation];
				if(iCollumn >= maxFrames){
					if(loop){
						std::get<1>(actualFrame) = 0;
					}
				}else{
					std::get<1>(actualFrame) += 1;
				}
				iCollumn = std::get<1>(actualFrame);
			}
			return {margin + padding*iCollumn + frameWidth*iCollumn, margin + padding*iRow + frameHeight*iRow,
				frameWidth, frameHeight};
		}
	
	private:
		int frameWidth, frameHeight, margin, padding;
		float frameDuration, actualDelta;
		std::unordered_map<std::string, std::tuple<int, int, bool>> animations; //First argument row, second argument max Frames of animation
		std::tuple<int, int> actualFrame; //First argument row, second argument collumn
		std::string actualAnimation;
};
