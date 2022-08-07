#pragma once

#include <array>
#include <unordered_map>
#include <assert.h>
#include <functional>

#include "IComponentArray.hpp"
#include "utils/type_name.hpp"

template<typename T>
class ComponentArray : public IComponentArray {
public:
	ComponentArray() = default;

	ComponentArray(const ComponentArray<T>& componentArray){
		mComponentArray = componentArray.mComponentArray;
		mEntityToIndexMap = componentArray.mEntityToIndexMap;
		mIndexToEntityMap = componentArray.mIndexToEntityMap;
		mSize = componentArray.mSize;
	}


	void InsertData(Entity entity, T component)
	{
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		++mSize;
	}

	void RemoveData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		--mSize;
	}

	T& GetData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

		// Return a reference to the entity's component
		return mComponentArray[mEntityToIndexMap[entity]];
	}

	bool hasComponent(Entity entity){
		return mEntityToIndexMap.find(entity) != mEntityToIndexMap.end();
	}


	Entity find(std::function<bool(const T&)> findLambda){
		for(int i=0; i<mSize; i++){
			if(findLambda(mComponentArray[i])){
				return mIndexToEntityMap[i];
			}
		}
		return NullEntity;
	}

	void EntityDestroyed(Entity entity) override
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
			// Remove the entity's component if it existed
			RemoveData(entity);
		}
	}

	IComponentArray* clone() override{
		return new ComponentArray<T>(*this);
	}

	~ComponentArray(){
		mEntityToIndexMap.clear();
		mIndexToEntityMap.clear();
	}

private:
	// The packed array of components (of generic type T),
	// set to a specified maximum amount, matching the maximum number
	// of entities allowed to exist simultaneously, so that each entity
	// has a unique spot.
	std::array<T, MAX_ENTITIES> mComponentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> mEntityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, Entity> mIndexToEntityMap;

	// Total size of valid entries in the array.
	size_t mSize;
};
