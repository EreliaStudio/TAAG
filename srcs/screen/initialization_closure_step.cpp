#include "screen/initialization_screen.hpp"

#include "event.hpp"

InitialisationScreen::InitializationClosureStep::InitializationClosureStep(TextArea& p_textArea) :
	_textArea(p_textArea)
{

}

void InitialisationScreen::InitializationClosureStep::onStart()
{
	_textArea.setText(L"Initialization completed !");
}

void InitialisationScreen::InitializationClosureStep::onPending()
{
	finish();
}

spk::StateMachine::Step::ID InitialisationScreen::InitializationClosureStep::onFinish()
{
	_textArea.setText(L"Game launching !");
	EventDispatcher::emit(Event::GameReady);
	return L"Idle";
}
