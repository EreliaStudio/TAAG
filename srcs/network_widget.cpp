#include "network_widget.hpp"

void ClientWidget::_onUpdateEvent(spk::UpdateEvent& p_event)
{
	if (Context::instance()->client.isConnected() == true)
	{
		Context::instance()->client.update();
	}
}

ClientWidget::ClientWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	_onDisconnectContract = Context::instance()->client.addOnDisconnectionCallback([&]() {
		spk::cout << L"Disconnected from server." << std::endl;
		EventDispatcher::emit(Event::LaunchClientConnection);
	});
}

ClientWidget::~ClientWidget()
{
	Context::instance()->client.disconnect();
}