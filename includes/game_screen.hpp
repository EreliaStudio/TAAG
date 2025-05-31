#pragma once

#include <sparkle.hpp>

class GameScreen : public spk::Screen
{
private:
	void _onGeometryChange() override;

public:
	GameScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};