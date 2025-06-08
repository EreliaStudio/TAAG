#pragma once

#include <sparkle.hpp>

#include "structure/world.hpp"

#include "network/serializable_object.hpp"

class Actor : public SerializableObject
{
public:
	using Contract = spk::ContractProvider::Contract;
	using Job = spk::ContractProvider::Job;

	enum class Shape
	{
		Triangle = 3,
		Square = 4,
		Pentagon = 5,
		Hexagon = 6,
		Octogon = 8,
		Circle = 20
	};

private:
	Shape _shape = Shape::Triangle;
	spk::Vector2 _position = spk::Vector2(0, 0);
	float _rotation = 0.0f;
	spk::Vector2 _scale = spk::Vector2(1, 1);
	spk::Color _color = spk::Color::white;

	spk::SafePointer<Chunk> _bindedChunk;
	spk::ContractProvider _onEditionContractProvider;

public:
	void setShape(const Shape& p_shape);
	const Shape& shape() const;
	void setPosition(const spk::Vector2& p_position);
	const spk::Vector2& position() const;
	void setRotation(float p_rotation);
	const float& rotation() const;
	void setScale(const spk::Vector2& p_scale);
	const spk::Vector2& scale() const;
	void setColor(const spk::Color& p_color);
	const spk::Color& color() const;

	Contract subscribeToEdition(const Job& p_job);

	void serialize(spk::Message& p_message) const override;
	void deserialize(const spk::Message& p_message) override;
	static void skip(const spk::Message& p_message);

	spk::SafePointer<Chunk> bindedChunk() const;
	void setBindedChunk(spk::SafePointer<Chunk> p_chunk);
};