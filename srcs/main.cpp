#include <sparkle.hpp>

#include "event_dispatcher.hpp"

#include "main_widget.hpp"

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	EventDispatcher::Instanciator eventDispatcherInstanciator;

	EventDispatcher::Contract closeApplicationContract = EventDispatcher::instance()->subscribe(Event::CloseApplication, [&]() {
		app.quit(0);
	});

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::instance()->emit(Event::EnterMenuHUD);

	return (app.run());
}