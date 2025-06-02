#pragma once

#include <sparkle.hpp>

#include "structure/context.hpp"

#include "network/network_override.hpp"

class ClientWidget : public spk::Widget
{
private:
	Context::Instanciator _contextInstanciator;
	
	Client::Contract _onDisconnectContract;

	void _onUpdateEvent(spk::UpdateEvent& p_event) override;

public:
	ClientWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
	~ClientWidget();
};