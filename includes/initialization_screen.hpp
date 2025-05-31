#pragma once

#include <sparkle.hpp>

#include "widget_override.hpp"

#include <future>

class InitialisationScreen final : public spk::Screen
{
private:
    spk::AnimationLabel _animationLabel;
    TextArea _loadingText;
    spk::JSON::File _configFile{L"resources/config.json"};

    spk::StateMachine _stateMachine;

	class ClientConnectionStep : public spk::StateMachine::Step
	{
	private:
		struct ConnectionResult
		{
			bool success = false;
			std::wstring errorMsg;
		};

		TextArea& _textArea;
		spk::Timer _timer{15000_ms};
		spk::JSON::File _configFile{L"resources/config.json"};

		std::future<ConnectionResult> _future;
		bool _connectionAttempted = false;
		bool _connectionFinished = false;
		ConnectionResult _lastResult;

		void _startConnection();

	public:
		ClientConnectionStep(TextArea& p_textArea);

		void onStart() override;
		void onPending() override;
		spk::StateMachine::Step::ID onFinish() override;
	};


    void _onGeometryChange() override;
    void _onUpdateEvent(spk::UpdateEvent&) override;

public:
    InitialisationScreen(const std::wstring& name, spk::SafePointer<spk::Widget> parent);
};