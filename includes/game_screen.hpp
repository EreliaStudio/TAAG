#pragma once

#include <sparkle.hpp>

class GameScreen : public spk::Screen
{
private:
	void _onGeometryChange() override;

	ActorListManager _actorListManager;

public:
	GameScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};