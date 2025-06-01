#pragma once

#include "network_override.hpp"

#include "actor.hpp"
#include "world.hpp"

struct Context : public spk::Singleton<Context>
{
	Client client;

	ActorMap::ID playerID = -1;
	ActorMap actorMap;
	Tilemap tilemap;
};