#pragma once

#include <sparkle.hpp>

#include "widget_override.hpp"

#include "network_override.hpp"

#include "event_dispatcher.hpp"

class MenuHUD : public spk::Screen
{
private:
	TextEdit _addressTextEdit;
	PushButton _joinButton;
	PushButton::Contract _joinButtonContract;

	PushButton _hostButton;
	PushButton::Contract _hostButtonContract;

	PushButton _exitButton;
	PushButton::Contract _exitButtonContract;

	void _onGeometryChange()
	{
		spk::Vector2Int availableSize = spk::Vector2Int::min(geometry().size / 2, {600, 200});
		spk::Vector2Int position = (geometry().size - availableSize) / 2;

		const spk::Vector2Int space = {10, 10};
		spk::Vector2Int lineSize = {availableSize.x, (availableSize.y - space.y * 2)/ 3};
	
		spk::Vector2Int editSize = {lineSize.x * 0.7, lineSize.y};
		spk::Vector2Int buttonEditSize = {lineSize.x * 0.3 - space.x, lineSize.y};

		_addressTextEdit.setGeometry(position, editSize);
		_joinButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 0 + spk::Vector2Int(editSize.x + space.x, 0), buttonEditSize);
		_hostButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 1, lineSize);
		_exitButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 2, lineSize);
	}

	void _onJoinRequest()
	{
		Client::instanciate();

		std::wstring address = _addressTextEdit.text();

		Client::instance()->setup(spk::StringUtils::wstringToString(address));
		Client::instance()->connect();
	}

	void _onHostRequest()
	{
		Server::instanciate();

		Server::instance()->start();

		Client::instanciate();

		Client::instance()->setup("localhost");
		Client::instance()->connect();
	}

public:
	MenuHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_addressTextEdit(p_name + L"/IpTextEdit", this),
		_joinButton(p_name + L"/JoinButton", this),
		_hostButton(p_name + L"/HostButton", this),
		_exitButton(p_name + L"/ExitButton", this)
	{
		_addressTextEdit.setPlaceholder(L"Enter IP address");
		_addressTextEdit.activate();

		_joinButton.setText(L"Join");
		_joinButtonContract = _joinButton.subscribe([&]() {
			_onJoinRequest();
		});
		_joinButton.activate();

		_hostButton.setText(L"Host game");
		_hostButtonContract = _hostButton.subscribe([&]() {
			_onHostRequest();
		});
		_hostButton.activate();

		_exitButton.setText(L"Quit");
		_exitButtonContract = _exitButton.subscribe([&]() {
			EventDispatcher::instance()->emit(Event::CloseApplication);
		});
		_exitButton.activate();
	}
};