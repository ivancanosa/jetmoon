#include "RenderAnimation.hpp"
#include <set>                                       // for set
#include <string>                                    // for operator==, basi...
#include <tuple>                                     // for get, tuple
#include <vector>                                    // for vector
#include "commonStructures/AnimationDefinition.hpp"  // for AnimationDefinition
#include "components/SpriteAnimation.hpp"            // for SpriteAnimation
#include "core/ServiceContext.hpp"                   // for ServiceContext
#include "core/Time.hpp"                             // for Time, Time::delt...
#include "core/World.hpp"                            // for World
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "services/AnimationDefinitionService.hpp"   // for AnimationDefinit...
#include "services/ImageLoader.hpp"                  // for ImageLoader
#include "services/Renderer2D.hpp"                   // for Renderer2D
struct WorldContext;

//This modifies the SpriteAnimation component
void nextAnimation(SpriteAnimation& animation, const AnimationDefinition& definition, float deltaTime, bool overrideDuration, float durationOv){
	if(definition.id == "") return;
	auto& vector = definition.subAnimations;
	auto& key = animation.subAnimation;
	auto it = std::find_if(
		vector.begin(), vector.end(),
		[&key](const auto& x) { return key == std::get<0>(x);});

	if(it == vector.end()) return; //TODO: Log incorrent subanimation
	animation.deltaTime += deltaTime/1000000;
	auto&[_, row, finalCollumn, d, loop] = *it;
	float duration = d;
	if(overrideDuration) duration = durationOv;
	float frameDuration = duration/finalCollumn;//TODO: What if finalCollumn is 0?

	if(row != animation.row){ //The animation has changed
		animation.deltaTime = 0;
		animation.row = row;
		animation.animationFrame = 0;
		return;
	}

	if(animation.deltaTime >= frameDuration){ //Compute next frame
		animation.deltaTime = 0;
		if(animation.animationFrame == finalCollumn && loop){
			animation.animationFrame = 0;
			return;
		}
		if(animation.animationFrame != finalCollumn){
			animation.animationFrame += 1;
		}
	}
}

std::tuple<float, float> animationToUV(const SpriteAnimation& animation, const AnimationDefinition& definition, int width, int height){
	float posX = definition.margin + definition.padding*animation.animationFrame + animation.animationFrame*definition.frameWidth;
	float posY = definition.margin + definition.padding*animation.row + animation.row*definition.frameHeight;
	return {posX/width, posY/height};
}

void RenderAnimation::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void RenderAnimation::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){

}

void RenderAnimation::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	char flipMap[2] {1, -1};
	char flipX, flipY;
	float scaleX, scaleY;
	float layer = 0.;

	AnimationDefinitionService* animationDefinitionService = serviceContext->animationDefinitionService.get();
	ImageLoader* imageLoader = serviceContext->imageLoader.get();

	glm::mat4 trans;
	glm::vec4 color{1.,1.,1.,1.};

	for(auto&& entity: this->mEntities){
		auto& animationComponent = world->getComponent<SpriteAnimation>(entity);
		if(!animationComponent.visible) continue;
		auto tr = world->reduceTransformMatrix(entity);
		auto animationDefinition = animationDefinitionService->getAnimationDefinition(animationComponent.animationController);
		layer += animationComponent.position.z;


		flipX = flipMap[animationComponent.flipX];
		flipY = flipMap[animationComponent.flipY];
		scaleX = flipX * animationComponent.size.x;
		scaleY = flipY * animationComponent.size.y;

		trans = glm::mat4(1.0f);

		trans = glm::translate(trans, glm::vec3(animationComponent.position.x, animationComponent.position.y, layer));
		trans = glm::rotate(trans, animationComponent.rotation, glm::vec3(0.f, 0.f, flipY*flipX*1.0f));
		trans = glm::scale(trans, glm::vec3(scaleX, scaleY, 1.f));

		auto[textureId, width, height] = imageLoader->loadWithSize(animationDefinition.id);
		nextAnimation(animationComponent, animationDefinition, Time::deltaTime, animationComponent.overrideDuration, animationComponent.duration);
		auto[u, v] = animationToUV(animationComponent, animationDefinition, width, height); 

		glm::vec4 textureClip{u, v, float(animationDefinition.frameWidth)/width, float(animationDefinition.frameHeight)/height};

		serviceContext->renderer2D->drawSprite(tr * trans, textureId, textureClip, color, 1., entity);
	}
	serviceContext->renderer2D->flushSprites();
}


std::string_view RenderAnimation::getName(){
	static std::string str{"RenderAnimation"};
	return str;
}

ISystem* RenderAnimation::clone(){
	return new RenderAnimation();
}
