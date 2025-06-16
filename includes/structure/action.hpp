#pragma once

#include <sparkle.hpp>

class Action
{
public:
	class Effect
	{
	public:
		enum class Type
		{

		};

	private:

	public:
		Effect()
		{

		}

	};

private:
	long long _cooldown = 16;
	std::vector<Effect> _effects;

public:
	void setCooldown(long long p_cooldown)
	{
		_cooldown = p_cooldown;
	}
	long long cooldown() const
	{
		return (_cooldown);
	}

	void addEffect(const Effect& p_effect)
	{
		_effects.push_back(p_effect);
	}
	void clearEffects()
	{
		_effects.clear();
	}

	virtual void serialize(spk::Message& p_msg) = 0;
	virtual void deserialize(const spk::Message& p_msg) = 0;
};