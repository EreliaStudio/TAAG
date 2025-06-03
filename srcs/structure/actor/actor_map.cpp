#include "structure/actor_map.hpp"

spk::SafePointer<Actor> ActorMap::addActor(ActorID id, std::unique_ptr<Actor> p_actor)
{
	_actors[id] = std::move(p_actor);
	return (actor(id));
}

bool ActorMap::contains(ActorID id) const
{
	return _actors.find(id) != _actors.end();
}

spk::SafePointer<Actor> ActorMap::actor(ActorID id) const
{
	return _actors.at(id).get();
}

spk::SafePointer<Actor> ActorMap::requestActor(ActorID id)
{
	if (contains(id) == false)
	{
		addActor(id, std::make_unique<Actor>());
	}
	return (actor(id));
}

void ActorMap::removeActor(ActorID id)
{
	_actors.erase(id);
}

const std::unordered_map<ActorMap::ActorID, std::unique_ptr<Actor>>& ActorMap::actors() const
{
	return _actors;
}

std::unordered_map<ActorMap::ActorID, std::unique_ptr<Actor>>& ActorMap::actors()
{
	return _actors;
}