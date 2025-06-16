#pragma once

#include <sparkle.hpp>

#include "structure/action.hpp"

class ActionMap : public spk::Singleton<ActionMap>
{
private:
	std::unordered_map<std::wstring, std::unique_ptr<Action>> _actions;

public:
	ActionMap()
	{

	}

	spk::SafePointer<const Action> action(const std::wstring& p_name)
	{
		if ()
	}
};