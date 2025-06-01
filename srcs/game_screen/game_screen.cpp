#include "game_screen.hpp"

#include "context.hpp"

void GameScreen::_onGeometryChange()
{
	_actorListManager.setGeometry(geometry());
}

GameScreen::GameScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Screen(p_name, p_parent),
	_actorListManager(p_name + L"/ActorListManager", this)
{
	_actorListManager.activate();
}