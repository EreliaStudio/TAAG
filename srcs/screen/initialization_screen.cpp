#include "screen/initialization_screen.hpp"

void InitialisationScreen::_onGeometryChange()
{
	spk::Vector2UInt spinSize = std::min(150u, geometry().size.y / 2);
	spk::Vector2UInt textSize = {geometry().size.x, geometry().size.y / 2};

	_animationLabel.setGeometry({{(geometry().size.x - spinSize.x) / 2, geometry().size.y / 2 - spinSize.y}, spinSize});
	_loadingText.setGeometry({{0, geometry().size.y / 2}, textSize});
}

void InitialisationScreen::_onUpdateEvent(spk::UpdateEvent&)
{
	_stateMachine.update();
}

InitialisationScreen::InitialisationScreen(const std::wstring& name, spk::SafePointer<spk::Widget> parent) :
	spk::Screen(name, parent),
	_loadingText(name + L"/LoadingText", this),
	_animationLabel(name + L"/AnimationLabel", this)
{
	_stateMachine.addStep(L"ClientConnection", std::make_unique<ClientConnectionStep>(_loadingText));
	_stateMachine.addStep(L"Idle", nullptr);

	addActivationCallback([&](){
		_stateMachine.setStep(L"ClientConnection");
	}).relinquish();

	_loadingText.setTextAlignment(spk::HorizontalAlignment::Centered, spk::VerticalAlignment::Top);
	_loadingText.setNineSlice(nullptr);
	_loadingText.activate();

	_animationLabel.setSpriteSheet(AssetAtlas::instance()->spriteSheet(L"SpinLoop"));
	_animationLabel.setAnimationRange(0, _animationLabel.spriteSheet()->sprites().size());
	_animationLabel.setLoopSpeed(125_ms);
	_animationLabel.activate();		
}