#pragma once

#include <sparkle.hpp>

class GameHUD : public spk::Screen
{
private:

public:
	GameHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{
		
	}
};