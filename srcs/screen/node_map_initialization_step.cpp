#include "screen/initialization_screen.hpp"

#include "structure/context.hpp"

InitialisationScreen::NodeMapInitializationStep::NodeMapInitializationStep(TextArea& p_textArea) :
	_textArea(p_textArea)
{
	
}

void InitialisationScreen::NodeMapInitializationStep::onStart()
{
	Context::instance()->nodeMap.addNode('w', Node(Node::Flag::Obstacle, Node::Type::Monotiled, {0, 0}, 1, {1, 0}));
	Context::instance()->nodeMap.addNode('W', Node(Node::Flag::Obstacle, Node::Type::Autotiled, {0, 0}, 1, {1, 0}));
}

void InitialisationScreen::NodeMapInitializationStep::onPending()
{
	finish();
}

spk::StateMachine::Step::ID InitialisationScreen::NodeMapInitializationStep::onFinish()
{
	_textArea.setText(L"Nodemap initialization complete !");
	return InitializationClosureStep::Name;
}
