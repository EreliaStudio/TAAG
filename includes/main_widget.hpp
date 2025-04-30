#pragma once

#include <sparkle.hpp>

#include "asset_atlas.hpp"
#include "event_dispatcher.hpp"

#include "game_hud.hpp"
#include "menu_hud.hpp"

class MainWidget : public spk::Widget
{
private:
	spk::JSON::File _configurationFile;
	AssetAtlas::Instanciator _assetAtlasInstanciator;
	EventDispatcher::Instanciator _eventDispatcherInstanciator;

	GameHUD _gameHUD;
	EventDispatcher::Contract _gameHUDContract;
	
	MenuHUD _menuHUD;
	EventDispatcher::Contract _menuHUDContract;

	void _onGeometryChange() override
	{
		_gameHUD.setGeometry(geometry());
		_menuHUD.setGeometry(geometry());
	}

public:
	MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent),
		_configurationFile(L"resources/configuration.json"),
		_assetAtlasInstanciator(spk::JSON::File(_configurationFile[L"AssetsConfig"].as<std::wstring>())),
		_gameHUD(p_name + L"/GameHUD", this),
		_menuHUD(p_name + L"/MenuHUD", this)
	{
		_gameHUDContract = EventDispatcher::instance()->subscribe(Event::OpenGameHUD, [&]() {
			_gameHUD.activate();
		});

		_menuHUDContract = EventDispatcher::instance()->subscribe(Event::OpenMenuHUD, [&]() {
			_menuHUD.activate();
		});
	}
};