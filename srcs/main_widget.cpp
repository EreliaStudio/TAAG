#include "main_widget.hpp"

void MainWidget::_onGeometryChange()
{
	_initialisationScreen.setGeometry({0, geometry().size});
	_gameScreen.setGeometry({0, geometry().size});
}

MainWidget::MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent),
	_clientWidget(p_name + L"/ClientWidget", this),
	_initialisationScreen(p_name + L"/InitialisationScreen", this),
	_gameScreen(p_name + L"/GameScreen", this)
{
	_initializationStartedContract = EventDispatcher::subscribe(Event::LaunchClientConnection, [&](){
		_initialisationScreen.activate();
	});

	_initializationFinishedContract = EventDispatcher::subscribe(Event::ClientConnected, [&](){
		_gameScreen.activate();
	});

	_clientWidget.activate();
}