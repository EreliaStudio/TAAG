#pragma once

#include <sparkle.hpp>

#include "structure/context.hpp"

#include "event.hpp"

#include "network/network_widget.hpp"

#include "screen/initialization_screen.hpp"
#include "screen/game_screen.hpp"

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