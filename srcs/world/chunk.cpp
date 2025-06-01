#include "world.hpp"

#include "actor.hpp"

Chunk::Chunk() : spk::IChunk<char, 16, 16, 3>()
{

}

void Chunk::bindActor(spk::SafePointer<Actor> p_actor)
{
	if (p_actor->bindedChunk() != nullptr)
	{
		p_actor->bindedChunk()->unbindActor(p_actor);
	}
	_bindedActors.insert(p_actor);
	p_actor->setBindedChunk(this);
}

void Chunk::unbindActor(spk::SafePointer<Actor> p_actor)
{
	_bindedActors.erase(p_actor);
	p_actor->setBindedChunk(nullptr);
}