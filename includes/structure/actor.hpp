#pragma once

#include <sparkle.hpp>

#include "structure/world.hpp"

#include "network/serializable_object.hpp"

class Actor : public SerializableObject
{
public:
	using Contract = spk::ContractProvider::Contract;
	using Job = spk::ContractProvider::Job;

private:
	spk::Vector2 _position = spk::Vector2(0, 0);
	float _rotation = 0.0f;
	spk::Vector2 _scale = spk::Vector2(1, 1);

	spk::SafePointer<Chunk> _bindedChunk;
	spk::ContractProvider _onEditionContractProvider;

public:
	void setPosition(const spk::Vector2& p_position);
	const spk::Vector2& position() const;
	void setRotation(float p_rotation);
	const float& rotation() const;
	void setScale(const spk::Vector2& p_scale);
	const spk::Vector2& scale() const;

	Contract subscribeToEdition(const Job& p_job);

	void serialize(spk::Message& p_message) const override;
	void deserialize(const spk::Message& p_message) override;
	static void skip(const spk::Message& p_message);

	spk::SafePointer<Chunk> bindedChunk() const;
	void setBindedChunk(spk::SafePointer<Chunk> p_chunk);
};