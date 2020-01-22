#pragma once

#include "Component.h"
#include "Pool.h"
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <cstdint>

class World;
class EntityManager;

class Entity
{

public:

	Entity(uint32_t index = 0, uint8_t version = 0) { id = (version << IndexBits) | index; }

	Entity(const Entity&) = default;
	Entity& operator=(const Entity&) = default;

	bool operator==(const Entity &e) const { return getIndex() == e.getIndex(); }
	bool operator!=(const Entity &e) const { return getIndex() != e.getIndex(); }
	bool operator<(const Entity &e) const { return getIndex() < e.getIndex(); }

	/* Entity lifetime functions. */
	void kill();
	bool isAlive() const;
	bool IsValid() const;
	/*------------------------*/

	/* Component related functions. */
	template <typename T> void addComponent(T component);
	template <typename T, typename ... Args> void addComponent(Args && ... args);
	template <typename T> void removeComponent();
	template <typename T> bool hasComponent() const;
	template <typename T> T& getComponent() const;
	/*------------------------*/

	/* Tag and gorup related functions */
	void tag(std::string tag);
	bool hasTag(std::string tag) const;

	void group(std::string group);
	bool hasGroup(std::string group) const;
	/*------------------------*/

	/* Returns the id as string (id + version).*/
	std::string toString() const;
	
	uint32_t getIndex() const { return id & IndexMask; }
	uint8_t getVersion() const { return (id >> IndexBits) & VersionMask; }

private:

	/* Unique index for entity.*/
	uint32_t id;

	/* Pointer to EntityManager to call all the utility funcitons.*/
	EntityManager *entityManager = nullptr;

	EntityManager& getEntityManager() const;

	static const uint32_t IndexBits = 24;
	static const uint32_t IndexMask = (1 << IndexBits) - 1;
	static const uint32_t VersionBits = 8;
	static const uint32_t VersionMask = (1 << VersionBits) - 1;
	
	friend class EntityManager;
};

class EntityManager
{
public:
	EntityManager(World &world) : world(world) {}

	/* Entity related functions. */
	Entity createEntity();
	void destroyEntity(Entity e);
	void killEntity(Entity e);
	bool isEntityAlive(Entity e) const;
	Entity getEntity(uint32_t index);
	/*------------------------*/

	/* Component related functions. */
	template <typename T> void addComponent(Entity e, T component);
	template <typename T, typename ... Args> void addComponent(Entity e, Args && ... args);
	template <typename T> void removeComponent(Entity e);
	template <typename T> bool hasComponent(Entity e) const;
	template <typename T> T& getComponent(Entity e) const;
	const ComponentMask& getComponentMask(Entity e) const;
	/*------------------------*/

	/* Tag related funtions.*/
	void tagEntity(Entity e, std::string tag);
	bool hasTag(std::string tag) const;
	bool hasTaggedEntity(std::string tag, Entity e) const;
	Entity getEntityByTag(std::string tag);
	int getTagCount() const;
	/*------------------------*/
	
	/* Group related funtions.*/
	void groupEntity(Entity e, std::string group);
	bool hasGroup(std::string group) const;
	bool hasEntityInGroup(std::string group, Entity e) const;
	std::vector<Entity> getEntityGroup(std::string group);
	int getGroupCount() const;
	/*------------------------*/

private:
	
	/* Each element of this vector will tell us all the components index's entity has.*/
	std::vector<ComponentMask> componentMasks;

	/* deque of free entity indices */
	std::deque<uint32_t> freeIds;

	/* vector of versions (index = entity index) */
	std::vector<uint8_t> versions;

	/* vector of component pools, each pool contains all the data for a certain component type*/
	/* vector index = component id, pool index = entity id */
	std::vector<std::shared_ptr<AbstractPool>> componentPools;

	/* maps for tag and groups. */
	std::unordered_map<std::string, Entity> taggedEntities;
	std::unordered_map<uint32_t, std::string> entityTags;
	std::unordered_map<std::string, std::set<Entity>> groupedEntities;
	std::unordered_map<uint32_t, std::string> entityGroups;
	/*------------------------*/

	template <typename T>
	std::shared_ptr<Pool<T>> accommodateComponent();

	World &world;
};

template <typename T>
void Entity::addComponent(T component)
{
	getEntityManager().addComponent<T>(*this, component);
}

template <typename T, typename ... Args>
void Entity::addComponent(Args && ... args)
{
	getEntityManager().addComponent<T>(*this, std::forward<Args>(args)...);
}

template <typename T>
void Entity::removeComponent()
{
	getEntityManager().removeComponent<T>(*this);
}

template <typename T>
bool Entity::hasComponent() const
{
	return getEntityManager().hasComponent<T>(*this);
}

template <typename T>
T& Entity::getComponent() const
{
	return getEntityManager().getComponent<T>(*this);
}



template <typename T>
void EntityManager::addComponent(Entity e, T component)
{
	const auto componentId = Component<T>::getId();
	const auto entityId = e.getIndex();
	std::shared_ptr<Pool<T>> componentPool = accommodateComponent<T>();

	if (entityId >= componentPool->getSize()) {
		componentPool->resize(versions.size());
	}

	componentPool->set(entityId, component);
	componentMasks[entityId].set(componentId);
}

template <typename T, typename ... Args>
void EntityManager::addComponent(Entity e, Args && ... args)
{
	T component(std::forward<Args>(args) ...);
	addComponent<T>(e, component);
}

template <typename T>
void EntityManager::removeComponent(Entity e)
{
	const auto componentId = Component<T>::getId();
	const auto entityId = e.getIndex();
	assert(entityId < componentMasks.size());
	componentMasks[entityId].set(componentId, false);
}

template <typename T>
bool EntityManager::hasComponent(Entity e) const
{
	const auto componentId = Component<T>::getId();
	const auto entityId = e.getIndex();
	assert(entityId < componentMasks.size());
	return componentMasks[entityId].test(componentId);
}

template <typename T>
T& EntityManager::getComponent(Entity e) const
{
	const auto componentId = Component<T>::getId();
	const auto entityId = e.getIndex();

	assert(hasComponent<T>(e));
	assert(componentId < componentPools.size());
	auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

	assert(componentPool);
	assert(entityId < componentPool->getSize());
	return componentPool->get(entityId);
}

template <typename T>
std::shared_ptr<Pool<T>> EntityManager::accommodateComponent()
{
	const auto componentId = Component<T>::getId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<T>> pool(new Pool<T>());
		componentPools[componentId] = pool;
	}

	return std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
}


