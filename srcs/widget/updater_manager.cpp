#include "widget/updater_manager.hpp"

#include "utils/uniform_buffer_object_atlas.hpp"

void UpdaterManager::_onUpdateEvent(spk::UpdateEvent& p_event)
{
	if (p_event.deltaTime.milliseconds == 0)
	{
		return ;
	}

	if (_startingTime == 0 || _startingTime > p_event.time.milliseconds)
	{
		_startingTime = p_event.time.milliseconds;
	}

	int epoch = p_event.time.milliseconds - _startingTime;
	
	_timeUBO[L"epoch"] = epoch;
	_timeUBO.validate();

	if (_needRenderTimer.state() != spk::Timer::State::Running)
	{
		requestPaint();
		_needRenderTimer.start();
	}
}

UpdaterManager::UpdaterManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent),
	_timeUBO(UniformBufferObjectAtlas::instance()->ubo(L"timeData"))
{
	
}