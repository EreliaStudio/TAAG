#pragma once

#include <sparkle.hpp>

class PlayerManager : public spk::Widget
{
private:
	void _parsePlayerIDAssignation(const spk::Message& p_message);

public:
	PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};