#include "screen/initialization_screen.hpp"

#include "structure/context.hpp"

void InitialisationScreen::ClientConnectionStep::_startConnection()
{
	_future = std::async(std::launch::async, [this]() -> ConnectionResult {
		ConnectionResult result;
		try
		{
			std::wstring server = _configFile[L"ServerAddress"].as<std::wstring>();
			Context::instance()->client.connect(spk::StringUtils::wstringToString(server), 26500);
			result.success = true;
		}
		catch (const std::exception& e)
		{
			result.success = false;
			result.errorMsg = spk::StringUtils::stringToWString(e.what());
		}
		return result;
	});
	_connectionAttempted = true;
}

InitialisationScreen::ClientConnectionStep::ClientConnectionStep(TextArea& p_textArea) :
	_textArea(p_textArea)
{
}

void InitialisationScreen::ClientConnectionStep::onStart()
{
	_textArea.setText(L"Connection to server started...");

	_connectionAttempted = false;
	_connectionFinished = false;

	_lastResult = {};

	_future = std::future<ConnectionResult>();

	_timer.stop();
	_startConnection();
}

void InitialisationScreen::ClientConnectionStep::onPending()
{
	if (_connectionFinished == true)
	{
		return;
	}

	std::wstring errorToShow = L"";
	if (_connectionAttempted == true && _future.valid())
	{
		using namespace std::chrono_literals;
		auto status = _future.wait_for(0ms);
		if (status == std::future_status::ready)
		{
			_lastResult = _future.get();
			if (_lastResult.success == true)
			{
				_connectionFinished = true;
				finish();
			}
			else
			{
				errorToShow = _lastResult.errorMsg;
				_timer.start();
				_connectionAttempted = false;
			}
		}
	}

	_textArea.setText(L"Failed to connect to server\n"
		"Retrying connection in [" + std::to_wstring(static_cast<int>((_timer.expectedDuration() - _timer.elapsed()).seconds)) + L"] seconds ...\n" +
		errorToShow);

	if (_connectionAttempted == false && _timer.state() != spk::Timer::State::Running)
	{
		_startConnection();
	}
}

spk::StateMachine::Step::ID InitialisationScreen::ClientConnectionStep::onFinish()
{
	_textArea.setText(L"Connection complete !");
	EventDispatcher::emit(Event::ClientConnected);
	return L"Idle";
}