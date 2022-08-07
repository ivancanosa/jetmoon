#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <any>

#include "definitions.hpp"
#include "IComponentArray.hpp"
#include "ComponentArray.hpp"

class ComponentManager {
public:
	ComponentManager(){}

	ComponentManager(const ComponentManager& componentManager){
		mComponentTypes = componentManager.mComponentTypes;
		for(auto const& pair: componentManager.mComponentArrays){
			std::shared_ptr<IComponentArray> s;
			s.reset(pair.second->clone());
			mComponentArrays.insert( {pair.first, s} );
		}
		mNextComponentType = componentManager.mNextComponentType;
	}

	template<typename T>
	void registerComponent() {
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		mComponentTypes.insert({typeName, mNextComponentType});

		// Create a ComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

		// Increment the value so that the next component registered will be different
		++mNextComponentType;
	}

	template<typename T>
	ComponentType getComponentType() {
		const char* typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return mComponentTypes[typeName];
	}

	template<typename T>
	void addComponent(Entity entity, T component) {
		// Add a component to the array for an entity
		GetComponentArray<T>()->InsertData(entity, component);
	}


	template<typename T>
	Entity find(std::function<bool(const T&)> findLambda){
		return GetComponentArray<T>()->find(findLambda);
	}

	template<typename T>
	void removeComponent(Entity entity) {
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& getComponent(Entity entity) {
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>()->GetData(entity);
	}

	template<typename T>
	bool hasComponent(Entity entity) {
		return GetComponentArray<T>()->hasComponent(entity);
	}

	void entityDestroyed(Entity entity) {
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : mComponentArrays) {
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

	std::vector<ComponentType> getAllComponents(Entity entity){
		std::vector<ComponentType> componentTypes{};
		for (auto const& pair : mComponentTypes) {
			componentTypes.emplace_back(pair.second);
		}
		return componentTypes;
	}

	std::unordered_map<const char*, ComponentType> clonemComponentTypes(){
		std::unordered_map<const char*, ComponentType> value{};
		for(const auto& pair: mComponentTypes){
				value.insert( {pair.first, pair.second} );
		}
		return value;
	}

	ComponentManager* clone(){
		return new ComponentManager(*this);
	}

	~ComponentManager(){
		mComponentTypes.clear();
		mComponentArrays.clear();
	}

private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type to be assigned to the next registered component - starting at 0
	ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray() {
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};
