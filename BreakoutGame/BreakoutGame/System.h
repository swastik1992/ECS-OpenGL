#pragma once

#include "Event.h"
#include "Entity.h"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

class SystemManager;
class World;

class System
{
public:
	virtual ~System() {}

	virtual void Init() {};
	virtual void Update(float dt) {};
	virtual void Render() {};
	
	template <typename T>
	void requireComponent();

	std::vector<Entity> getEntities() { return entities; }

	void addEntity(Entity e);

	void removeEntity(Entity e);

	const ComponentMask& getComponentMask() const { return componentMask; }

protected:
	World& getWorld() const;

private:

	ComponentMask componentMask;

	std::vector<Entity> entities;

	World *world = nullptr;
	friend class SystemManager;
};

class SystemManager
{
public:
	SystemManager(World &world) : world(world) {}

	template <typename T>
	void addSystem();

	template <typename T, typename ... Args>
	void addSystem(Args && ... args);

	template <typename T>
	void removeSystem();

	template <typename T>
	T& getSystem();

	template <typename T>
	bool hasSystem() const;

	// adds an entity to each system that is interested of the entity
	void addToSystems(Entity e);

	// removes an entity from interested systems' entity lists
	void removeFromSystems(Entity e);

	void Init();
	void Update(float dt);
	void Render();

private:
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	World &world;
};

template <typename T>
void System::requireComponent()
{
	const auto componentId = Component<T>::getId();
	componentMask.set(componentId);
}

template <typename T>
void SystemManager::addSystem()
	{
		if (hasSystem<T>()) {
			return;
		}

		std::shared_ptr<T> system(new T);
		system->world = &world;
		systems.insert(std::make_pair(std::type_index(typeid(T)), system));
	}

template <typename T, typename ... Args>
void SystemManager::addSystem(Args && ... args)
	{
		if (hasSystem<T>()) {
			return;
		}

		std::shared_ptr<T> system(new T(std::forward<Args>(args) ...));
		system->world = &world;
		systems.insert(std::make_pair(std::type_index(typeid(T)), system));
	}

template <typename T>
void SystemManager::removeSystem()
	{
		if (!hasSystem<T>()) {
			return;
		}

		auto it = systems.find(std::type_index(typeid(T)));
		systems.erase(it);
	}

template <typename T>
T& SystemManager::getSystem()
	{
		if (!hasSystem<T>()) {
			throw std::runtime_error(std::string("Failed to get system: ") + typeid(T).name());
		}

		auto it = systems.find(std::type_index(typeid(T)));
		return *(std::static_pointer_cast<T>(it->second));
	}

template <typename T>
bool SystemManager::hasSystem() const
	{
		return systems.find(std::type_index(typeid(T))) != systems.end();
	}


