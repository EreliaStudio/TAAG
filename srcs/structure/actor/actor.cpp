#include "structure/actor.hpp"

void Actor::setShape(const Shape& p_shape)
{
	_shape = p_shape;	
}

const Actor::Shape& Actor::shape() const
{
	return (_shape);
}

void Actor::setPosition(const spk::Vector2& p_position)
{
	_position = p_position;
}

const spk::Vector2& Actor::position() const
{
	return _position;
}

void Actor::setRotation(float p_rotation)
{
	_rotation = p_rotation;
}

const float& Actor::rotation() const
{
	return _rotation;
}

void Actor::setScale(const spk::Vector2& p_scale)
{
	_scale = p_scale;
}

const spk::Vector2& Actor::scale() const
{
	return _scale;
}

void Actor::setColor(const spk::Color& p_color)
{
	_color = p_color;
}

const spk::Color& Actor::color() const
{
	return (_color);
}

spk::ContractProvider::Contract Actor::subscribeToEdition(const spk::ContractProvider::Job& p_job)
{
	return (_onEditionContractProvider.subscribe(p_job));
}

void Actor::serialize(spk::Message& p_message) const
{
	p_message << _shape;
	p_message << _position;
	p_message << _rotation;
	p_message << _scale;
	p_message << _color;
}

void Actor::deserialize(const spk::Message& p_message)
{
	p_message >> _shape;
	p_message >> _position;
	p_message >> _rotation;
	p_message >> _scale;
	p_message >> _color;
}

void Actor::skip(const spk::Message& p_message)
{
	p_message.skip<Shape>();
	p_message.skip<spk::Vector2>();
	p_message.skip<float>();
	p_message.skip<spk::Vector2>();
	p_message.skip<spk::Color>();
}

spk::SafePointer<Chunk> Actor::bindedChunk() const
{
	return _bindedChunk;
}

void Actor::setBindedChunk(spk::SafePointer<Chunk> p_chunk)
{
	_bindedChunk = p_chunk;
}