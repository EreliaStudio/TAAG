#pragma once

#include <sparkle.hpp>

#include "widget/graphical_widget.hpp"

class PlayerManager : public GraphicalWidget
{
private:
	void _parsePlayerIDAssignation(const spk::Message& p_message);

	void _onPaintEvent(spk::PaintEvent& p_event);

public:
	PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};