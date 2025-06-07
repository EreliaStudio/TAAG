#pragma once

#include <sparkle.hpp>

#include "widget/actor_list_manager.hpp"
#include "widget/world_manager.hpp"
#include "widget/player_manager.hpp"
#include "widget/updater_manager.hpp"

class GameScreen : public spk::Screen
{
private:
	void _onGeometryChange() override;

	ActorListManager _actorListManager;
	WorldManager _worldManager;
	PlayerManager _playerManager;
	UpdaterManager _updaterManager;

public:
	GameScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};