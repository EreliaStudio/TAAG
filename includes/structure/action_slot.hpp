#pragma once

#include "structure/action.hpp"

class ActionSlot
{
private:
	spk::SafePointer<const Action> _action;
	spk::Timer _cooldownTimer;

public:
	void setAction(spk::SafePointer<const Action> p_action)
	{
		_action = p_action;
		if (_action != nullptr)
		{
			_cooldownTimer.setDuration(spk::Duration(_action.cooldown, spk::TimeUnit::Millisecond));
		}
	}

	spk::SafePointer<const Action> action() const
	{
		return (_action);
	}

	void cast()
	{
		_cooldownTimer.start();
	}

	void reset()
	{
		_cooldownTimer.stop();
	}

	const spk::Timer& cooldown() const
	{
		return (_cooldownTimer);
	}
};