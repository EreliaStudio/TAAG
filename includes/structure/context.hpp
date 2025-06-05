#pragma once

#include <sparkle.hpp>

#include "network/network_override.hpp"

#include "structure/actor_map.hpp"
#include "structure/world.hpp"

struct Context : public spk::Singleton<Context>
{
	Client client;

	NodeMap nodeMap;
	ActorMap::ActorID playerID = -1;
	ActorMap actorMap;
	Tilemap tilemap;
};