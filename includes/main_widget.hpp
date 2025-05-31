#pragma once

#include <sparkle.hpp>

#include "context.hpp"

#include "event.hpp"

#include "network_widget.hpp"

#include "initialization_screen.hpp"
#include "game_screen.hpp"

class MainWidget : public spk::Widget
{
private:
	Context::Instanciator _contextInstanciator;

	EventDispatcher::Contract _initializationStartedContract;
	EventDispatcher::Contract _initializationFinishedContract;

	ClientWidget _clientWidget;

	InitialisationScreen _initialisationScreen;
	GameScreen _gameScreen;

	void _onGeometryChange() override;

public:
	MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};