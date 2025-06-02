#include "screen/game_screen.hpp"

#include "structure/context.hpp"

void GameScreen::_onGeometryChange()
{
	_actorListManager.setGeometry(geometry());
	_worldManager.setGeometry(geometry());
	_playerManager.setGeometry(geometry());
}

GameScreen::GameScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Screen(p_name, p_parent),
	_actorListManager(p_name + L"/ActorListManager", this),
	_worldManager(p_name + L"/WorldManager", this),
	_playerManager(p_name + L"/PlayerManager", this)
{
	_worldManager.activate();
	_actorListManager.activate();
	_playerManager.activate();
}