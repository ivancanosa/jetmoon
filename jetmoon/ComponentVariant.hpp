#include "components/Hierarchy.hpp"
#include "components/NameComponent.hpp"
#include "components/Transform.hpp"
#include "components/Sprite.hpp"
#include "components/SpriteAnimation.hpp"
#include "components/Trail.hpp"
#include "components/LightSource.hpp"
#include "components/CameraComponent.hpp"
#include "components/NullComponent.hpp"
#include "components/RigidBody2D.hpp"
#include "components/CollisionSensor.hpp"
#include "components/TilemapComponent.hpp"
#include "components/ParticleEmisor.hpp"
#include "components/AudioSource.hpp"
#include "components/MovementVector.hpp"
#include "components/PostProcessing.hpp"
#include "components/ProceduralTexture.hpp"
#include "components/PrefabComponent.hpp"
#include <variant>


using ComponentVariant = std::variant<
	Hierarchy,
	PrefabComponent,
	NameComponent,
	Transform,
	Sprite,
	SpriteAnimation,
	Trail,
	LightSource,
	ProceduralTexture,
	TilemapComponent,
	ParticleEmisor,
	RigidBody2D,
	CollisionSensor,
	MovementVector,
	AudioSource,
	CameraComponent,
	PostProcessing
>;
