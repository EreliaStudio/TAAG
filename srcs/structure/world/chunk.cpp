#include "structure/world.hpp"

#include "structure/actor.hpp"

Chunk::Chunk() : spk::IChunk<char, 16, 16, 3>()
{

}

void Chunk::setPosition(const spk::Vector2Int& p_position)
{
	_position = p_position;
}

const spk::Vector2Int& Chunk::position()
{
	return (_position);
}

void Chunk::serialize(spk::Message& p_message) const
{
	p_message << contentArray();
}

void Chunk::deserialize(const spk::Message& p_message)
{
	p_message >> contentArray();
}

void Chunk::skip(const spk::Message& p_message)
{
	p_message.skip<NodeMap::ID[16][16][3]>();
}

bool Chunk::isBaked() const
{
	return (_isBaked);
}

void Chunk::bake()
{
	if (_isBaked == true)
	{
		return ;
	}

	_renderer.clear();

	for (int x = 0; x < Size.x; x++)
	{
		for (int y = 0; y < Size.y; y++)
		{
			for (int z = 0; z < Size.z; z++)
			{
				Chunk::ContentType index = content(x, y, z);
				if (Context::instance()->nodeMap.contains(index) == true)
				{
					
				}
			}	
		}	
	}

	_renderer.validate();

	_isBaked = true;
}

void Chunk::render()
{
	_renderer.render();
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

const std::set<spk::SafePointer<Actor>>& Chunk::bindedActors() const
{
	return _bindedActors;
}