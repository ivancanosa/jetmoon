#pragma once

#include <memory>
#include <map>
#include <functional>
#include <set>
#include <typeinfo>

#include <tuple>

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

#include "core/Time.hpp"
#include "core/WorldContextOperations.hpp"
#include "commonStructures/EngineStatistics.hpp"

#include "components/Hierarchy.hpp"
#include "components/Transform.hpp"

#include "utils/type_name.hpp"
#include "utils/variant_by_index.hpp"
#include "utils/Stopwatch.hpp"

struct ServiceContext;
struct WorldContext;

class World {
public:
	World(){
		worldContext = createWorldContext();
	}

	World(const World& world){
		if(world.loaded){
			mComponentManager.reset(world.mComponentManager->clone());
			mEntityManager.reset(world.mEntityManager->clone());
			mSystemManager.reset(world.mSystemManager->clone());
			timelines = world.timelines;
			rootEntities = world.rootEntities;
			serviceContext = world.serviceContext;
			for(const auto&[timelineStr, _]: world.systems){
				std::vector<std::shared_ptr<ISystem>> systemsVector{};

				for(auto& system: world.systems.at(timelineStr)){
					std::shared_ptr<ISystem> s;
					s.reset(system->clone());
					s->mEntities = system->mEntities;

					systemsVector.emplace_back(s);
					for(auto&[key, value]: world.mSystemManager.get()->mSystems){
						auto& type0 = *value.get();
						auto& type1 = *s.get();
						if(typeid(type0) == typeid(type1)){
							mSystemManager->mSystems.insert({key, s});
							break;
						}

					}
				}
				systems[timelineStr] = systemsVector;
			}
			entityToUUID = world.entityToUUID;
		}
		activeTimeline = world.activeTimeline;
		worldContext = cloneWorldContext(world.worldContext);
		componentLoader = world.componentLoader;
		systemLoader = world.systemLoader;
		entitiesLoader = world.entitiesLoader;
	}

	void setComponentLoader(std::function<void(World*)> loader){
		componentLoader = loader;
	}

	void setSystemLoader(std::function<void(World*)> loader){
		systemLoader = loader;
	}

	void setEntitiesLoader(std::function<void(World*)> loader){
		entitiesLoader = loader;
	}

	//Get here a reference to services
	void load(ServiceContext* serviceContext) {
		// Create pointers to each manager
		this->serviceContext = serviceContext;
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();

		componentLoader(this);
		systemLoader(this);
		entitiesLoader(this);
		loaded = true;
	}

	void unload(){
		delete mComponentManager.release();
		delete mEntityManager.release();
		delete mSystemManager.release();

		timelines.clear();
		systems.clear();
		loaded = false;
	}

	// Entity methods
	Entity createEntityInternal(UUID uuid = createUUID()) {
		auto entity = mEntityManager->CreateEntity();
		rootEntities.insert(entity);
		entityToUUID[entity] = uuid;
		return entity;
	}

	bool isEntityAlive(Entity entity){
		return entityToUUID[entity] != 0;
	}

	Entity createEntity(UUID uuid = createUUID()) {
		auto entity = mEntityManager->CreateEntity();
		addComponent<Hierarchy>(entity, {});
		addComponent<Transform>(entity, {});
		rootEntities.insert(entity);
		entityToUUID[entity] = uuid;
		return entity;
	}

	UUID getUUID(Entity entity){
		return entityToUUID[entity];
	}

	Entity getEntityByUUID(UUID uuid){
		for(int i=0; i < MAX_ENTITIES; i++){
			if(entityToUUID[i] == uuid){
				return i;
			}
		}
		return NullEntity;
	}

	void destroyAllEntities(){
		auto rootCopies = rootEntities; 
		for(auto entity: rootCopies){
			destroyEntity(entity);
		}
	}

	void destroyEntity(Entity entity, bool isRoot=true) {
		if(entityToUUID[entity] == 0) return;
		mSystemManager->entityDestroyed(entity, this, worldContext.get(), serviceContext);
		mEntityManager->DestroyEntity(entity);
		mComponentManager->entityDestroyed(entity);
		if(hasComponent<Hierarchy>(entity)){	
			auto& hierarchy = getComponent<Hierarchy>(entity);
			for(auto i: hierarchy.children){
				destroyEntity(i, false); 
			}
			if(hierarchy.parent != NullEntity && isRoot){
				auto& h = getComponent<Hierarchy>(hierarchy.parent);
				h.children.erase(entity);
			}
		}
		rootEntities.erase(entity);
		entityToUUID[entity] = 0;
	}

	const std::set<Entity>& getRootEntitiesSet(){
		return rootEntities;
	}

	// Component methods
	template<typename T>
	void registerComponent() {
		mComponentManager->registerComponent<T>();
	}

	template<typename T>
	void addComponent(Entity entity, T component) {
		assert(!hasComponent<T>(entity) && "The entity already has the component");
		mComponentManager->addComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->getComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->entitySignatureChanged(entity, signature, this, worldContext.get(), serviceContext);
	}

	//TODO: Find method in funciton of an component
	template<typename T>
	Entity find(std::function<bool(const T&)> findLambda){
		return mComponentManager->find(findLambda);
	}


	template<typename T>
	void removeComponent(Entity entity) {
		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->getComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->entitySignatureChanged(entity, signature, this, worldContext.get(), serviceContext);

		mComponentManager->removeComponent<T>(entity);
	}

	template<>
	void removeComponent<Transform>(Entity entity){
		assert(false && "Error, you can't remove a Transform component");
	}


	template<>
	void removeComponent<Hierarchy>(Entity entity){
		assert(false && "Error, you can't remove a Hierarchy component");
	}


	template<typename T>
	T& getComponent(Entity entity) {
		return mComponentManager->getComponent<T>(entity);
	}

	template<typename T>
	bool hasComponent(Entity entity) {
		return mComponentManager->hasComponent<T>(entity);
	}


	template<typename T>
	ComponentType getComponentType() {
		return mComponentManager->getComponentType<T>();
	}


	// System methods

	void addTimeline(std::string timeline, bool isActive = true){
		timelines.push_back(timeline);
		systems[timeline] = std::vector<std::shared_ptr<ISystem>>();
		if(activeTimeline.find(timeline) == activeTimeline.end()){
			activeTimeline[timeline] = isActive;
		}
	}

	void setTimelineState(std::string timeline, bool isActive){
		activeTimeline[timeline] = isActive;
	}

// example use: registerSystem<GravitySystem, Position, Velocity, Aceleration>("updateTimeline")
	template<typename S, typename... C>
	std::shared_ptr<S> registerSystem(std::string timeline) {
		Signature signature;
		assert(std::find(timelines.begin(), timelines.end(), timeline) != timelines.end() && "Timeline not created");
		auto system = mSystemManager->registerSystem<S>();
		registerComponent<C...>(&signature);
		mSystemManager->setSignature<S>(signature);
		systems[timeline].emplace_back(system);
		system->init(this, worldContext.get(), serviceContext);
		return system;
	}

	void update(ServiceContext* serviceContext, EngineStatistics* engineStatistics){
		Stopwatch stopwatch;
		step += 1;
		Time::frameCount = step;
		engineStatistics->coreEngineStatistics.entitiesCount = mEntityManager->getEntityCount();
		for(auto&& timeline: timelines){
			if(activeTimeline[timeline]){
				for(auto&& system: systems[timeline]){
					stopwatch.start();
					system->update(this, worldContext.get(), serviceContext);
					auto time = stopwatch.getTime();
					std::string str{system->getName()};
					engineStatistics->coreEngineStatistics.systemTime.push_back({str, float(time)/1000});
				}
			}
		}
	}

// Hierarchy functions

	bool isDescendant(Entity parent, Entity descendant){
		if(parent == descendant) return true;
		if(!hasComponent<Hierarchy>(parent)){
			return false;
		}
		if(!hasComponent<Hierarchy>(descendant)){
			return false;
		}
		auto& children = getComponent<Hierarchy>(parent).children;
		for(auto child: children){
			if(isDescendant(child, descendant)){
				return true;
			}
		}
		return false;
	}

	void disownEntity(Entity entity){
		if(hasComponent<Hierarchy>(entity)){
			auto& childHierarchy = getComponent<Hierarchy>(entity);
			if(childHierarchy.parent != NullEntity){
				auto& parentHierarchy = getComponent<Hierarchy>(childHierarchy.parent);	
				parentHierarchy.children.erase(entity);
			}
			childHierarchy.parent = NullEntity;
			rootEntities.insert(entity);
		}
	}

	void addChild(Entity parentID, Entity childID){
		if(isDescendant(childID, parentID)) return; //TODO: Logging failed attempt

		if(!hasComponent<Hierarchy>(parentID)){
			addComponent<Hierarchy>(parentID, {});
		}
		if(!hasComponent<Hierarchy>(childID)){
			addComponent<Hierarchy>(childID, {});
		}
		auto& parentHierarchy = getComponent<Hierarchy>(parentID);
		auto& childHierarchy = getComponent<Hierarchy>(childID);
		if(childHierarchy.parent != NullEntity){
			auto& parentHierarchyOld = getComponent<Hierarchy>(childHierarchy.parent);
			parentHierarchyOld.children.erase(childID);
		}
		parentHierarchy.children.insert(childID);
		childHierarchy.parent = parentID;
		rootEntities.erase(childID);
	}

	Entity getParent(Entity entity){
		assert(hasComponent<Hierarchy>(entity) && "The entity does not have the hierarchy component");
		return getComponent<Hierarchy>(entity).parent;
	}

	const std::unordered_set<Entity>& getChildren(Entity entity){
		assert(hasComponent<Hierarchy>(entity) && "The entity does not have the hierarchy component");
		return getComponent<Hierarchy>(entity).children;
	}

/*	Transform& reduce(Transform& a){
		auto tr1 = this->toMatrix();
		auto tr2 = a.toMatrix();
		auto tr3 = tr1 * tr2;

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(tr3), matrixTranslation, matrixRotation, matrixScale);
		position.x = matrixTranslation[0];
		position.y = matrixTranslation[1];
		position.z = matrixTranslation[2];
		scale.x = matrixScale[0];
		scale.y = matrixScale[1];
		rotation = matrixRotation[0]*2.*M_PI/360;

		position += a.position;
		scale *= a.scale;
		rotation += a.rotation;
		rotation = rotation - 2 * M_PI * floor(rotation / (2*M_PI));
		return *this;
	}*/




	template <class T>
	T reduceComponent(Entity actualEntity){
		if(!hasComponent<Hierarchy>(actualEntity)){
			if(hasComponent<T>(actualEntity)){
				return getComponent<T>(actualEntity);
			}else{
				return {};
			}
		}
		T acc{};
		while(actualEntity != NullEntity){
			if(!hasComponent<T>(actualEntity)){
				return acc;
			}
			auto& hierarchy = getComponent<Hierarchy>(actualEntity);
			auto& newT = getComponent<T>(actualEntity);
			acc.reduce(newT);
			actualEntity = hierarchy.parent;
		}
		return acc;
	}

	template <>
	Transform reduceComponent<Transform>(Entity actualEntity){
		glm::mat4 transformation = glm::mat4(1.f);

		if(!hasComponent<Hierarchy>(actualEntity)){
			if(hasComponent<Transform>(actualEntity)){
				return getComponent<Transform>(actualEntity);
			}else{
				return {};
			}
		}
		while(actualEntity != NullEntity){
			if(!hasComponent<Transform>(actualEntity)){
				return fromMatrix(transformation);
			}
			auto& hierarchy = getComponent<Hierarchy>(actualEntity);
			transformation = toMatrix(getComponent<Transform>(actualEntity)) * transformation;
			actualEntity = hierarchy.parent;
		}
		return fromMatrix(transformation);
	}

	glm::mat4 reduceTransformMatrixParent(Entity actualEntity){
		auto& hierarchy = getComponent<Hierarchy>(actualEntity);
		Entity parent = hierarchy.parent;
		if(parent != NullEntity){
			return reduceTransformMatrix(parent);
		}
		return glm::mat4(1.);

	}

	glm::mat4 reduceTransformMatrix(Entity actualEntity){
		if(actualEntity == NullEntity) return glm::mat4(1.);
		auto[transformation, s] = transformationMatrixCache[actualEntity];
		if(step == s) return transformation;
		transformation = toMatrix(getComponent<Transform>(actualEntity));
		
/*		while(actualEntity != NullEntity){
			if(!hasComponent<Transform>(actualEntity)){
				return transformation;
			}
			auto& hierarchy = getComponent<Hierarchy>(actualEntity);
			transformation = toMatrix(getComponent<Transform>(actualEntity)) * transformation;
			actualEntity = hierarchy.parent;
		}*/

		auto& hierarchy = getComponent<Hierarchy>(actualEntity);
		Entity parent = hierarchy.parent;
		if(parent != NullEntity){
			transformation = reduceTransformMatrix(parent) * transformation;
		}

		transformationMatrixCache[actualEntity] = {transformation, step};
		return transformation;
	}

	std::shared_ptr<World> clone(){
		auto newWorld = std::make_shared<World>(*this);
		return newWorld;
	}

	~World(){
		this->unload();
	}

	std::shared_ptr<WorldContext> worldContext{nullptr};
private:

	template<typename... Ts>
	struct registerComponentDS {
		static void call(Signature* signature, World* world) { }
	};
	template<typename T, typename... Ts>
	struct registerComponentDS<T, Ts...> {
		static void call(Signature* signature, World* world) {
			signature->set(world->getComponentType<T>());
			registerComponentDS<Ts...>::call(signature, world);
		}
	};
	template<typename... Ts>
	void registerComponent(Signature* signature) {
		registerComponentDS<Ts...>::call(signature, this);
	}

	std::unique_ptr<ComponentManager> mComponentManager{};
	std::unique_ptr<EntityManager> mEntityManager{};
	std::unique_ptr<SystemManager> mSystemManager{};

	std::map<std::string, std::vector<std::shared_ptr<ISystem>>> systems{};

	std::vector<std::string> timelines{};
	std::unordered_map<std::string, bool> activeTimeline{};

	std::function<void(World*)> componentLoader{};
	std::function<void(World*)> systemLoader{};
	std::function<void(World*)> entitiesLoader{};

	std::set<Entity> rootEntities{}; 
	ServiceContext* serviceContext{};

	std::array<UUID, MAX_ENTITIES> entityToUUID{0};

	std::array<std::tuple<glm::mat4, int>, MAX_ENTITIES> transformationMatrixCache{};
	unsigned int step{0};

	bool loaded{false};
};


//This asumes that all parents of childs have the componente T, and T have a function for reduce
//TODO: Implement a cache with the Entity as key that flush every frame

