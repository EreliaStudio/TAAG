#pragma once

#include <sparkle.hpp>

class SerializableObject
{
public:
	virtual ~SerializableObject() = default;

	virtual void serialize(spk::Message& p_message) const = 0;
	virtual void deserialize(const spk::Message& p_message) = 0;
};