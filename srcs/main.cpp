#include <sparkle.hpp>

#include "main_widget.hpp"

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::emit(Event::LaunchClientConnection);

	return (app.run());
}