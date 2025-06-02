#include "structure/actor.hpp"

void Actor::setPosition(const spk::Vector2& p_position)
{
	_position = p_position;
}

spk::Vector2 Actor::position() const
{
	return _position;
}

void Actor::setRotation(float p_rotation)
{
	_rotation = p_rotation;
}

float Actor::rotation() const
{
	return _rotation;
}

void Actor::setScale(const spk::Vector2& p_scale)
{
	_scale = p_scale;
}

spk::Vector2 Actor::scale() const
{
	return _scale;
}

void Actor::serialize(spk::Message& p_message) const
{
	p_message << _position;
	p_message << _rotation;
	p_message << _scale;
}

void Actor::deserialize(const spk::Message& p_message)
{
	p_message >> _position;
	p_message >> _rotation;
	p_message >> _scale;
}

void Actor::skip(const spk::Message& p_message)
{
	p_message.skip<spk::Vector2>();
	p_message.skip<float>();
	p_message.skip<spk::Vector2>();
}

spk::SafePointer<Chunk> Actor::bindedChunk() const
{
	return _bindedChunk;
}

void Actor::setBindedChunk(spk::SafePointer<Chunk> p_chunk)
{
	_bindedChunk = p_chunk;
}