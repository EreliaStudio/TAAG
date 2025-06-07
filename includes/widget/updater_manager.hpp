#pragma once

#include <sparkle.hpp>

class UpdaterManager : public spk::Widget
{
private:
	spk::OpenGL::UniformBufferObject& _timeUBO;

	long long _startingTime = 0;

	spk::Timer _needRenderTimer = spk::Timer(16_ms);

	void _onUpdateEvent(spk::UpdateEvent& p_event);

public:
	UpdaterManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};