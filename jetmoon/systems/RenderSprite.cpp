#include "RenderSprite.hpp"
#include <iosfwd>                        // for string
#include <set>                           // for set
#include "GL/glew.h"                     // for GLuint
#include "components/Sprite.hpp"         // for Sprite
#include "core/ServiceContext.hpp"       // for ServiceContext
#include "core/World.hpp"                // for World
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp> 
#include "services/ImageLoader.hpp"      // for ImageLoader
#include "services/Renderer2D.hpp"       // for Renderer2D
struct WorldContext;


std::string_view RenderSprite::getName(){
	static std::string str{"RenderSprite"};
	return str;
}

ISystem* RenderSprite::clone(){
	return new RenderSprite();
}

void RenderSprite::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void RenderSprite::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){ }

void RenderSprite::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	char flipMap[2] {1, -1};
	char flipX, flipY;
	float scaleX, scaleY;
	float layer = 0.;

	auto loader = serviceContext->imageLoader.get();

	glm::mat4 trans;
	float emisor{0.};
	for(auto&& entity: this->mEntities){
		auto tr = world->reduceTransformMatrix(entity);
		auto sprite = world->getComponent<Sprite>(entity);
		if(!sprite.visible) continue;
		if(sprite.emisor){
			emisor = 1.;
		}else{
			emisor = 0.;
		}
		layer =  sprite.position.z;

		flipX = flipMap[sprite.flipX];
		flipY = flipMap[sprite.flipY];
		scaleX = flipX  * sprite.size.x;
		scaleY = flipY  * sprite.size.y;
		//scale translate rotate
		trans = glm::mat4(1.0f);

		trans = glm::translate(trans, glm::vec3((sprite.position.x), (sprite.position.y), layer));
		trans = glm::rotate(trans, sprite.rotation, glm::vec3(0.f, 0.f, flipY*flipX*1.0f));
		trans = glm::scale(trans, glm::vec3(scaleX, scaleY, 1.f));

		glm::vec4 c = {emisor+sprite.color.x, emisor+sprite.color.y, emisor+sprite.color.z, emisor+sprite.color.w};
		GLuint textureID = loader->load(sprite.id);
		serviceContext->renderer2D->drawSprite(tr * trans, textureID, {0., 0., 1., 1.}, c, sprite.tiling, entity);
	}
	serviceContext->renderer2D->flushSprites();
}
