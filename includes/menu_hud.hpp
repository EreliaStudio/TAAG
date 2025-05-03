#pragma once

#include <sparkle.hpp>

#include "widget_override.hpp"

#include "event_dispatcher.hpp"

#include "context.hpp"

class MenuHUD : public spk::Screen
{
private:
	Context::Instanciator _contextInstanciator;

	void _onGeometryChange()
	{
		
	}

public:
	MenuHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{
		
	}
};