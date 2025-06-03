#pragma once

#include <unordered_map>
#include <memory>

#include <sparkle.hpp>

#include "structure/actor.hpp"

class ActorMap
{
public:
	using ActorID = long;

private:
	std::unordered_map<ActorID, std::unique_ptr<Actor>> _actors;

public:
	spk::SafePointer<Actor> addActor(ActorID id, std::unique_ptr<Actor> p_actor);

	bool contains(ActorID id) const;

	spk::SafePointer<Actor> actor(ActorID id) const;
	spk::SafePointer<Actor> requestActor(ActorID id);

	void removeActor(ActorID id);

	const std::unordered_map<ActorID, std::unique_ptr<Actor>>& actors() const;
	std::unordered_map<ActorID, std::unique_ptr<Actor>>& actors();
};