#include "widget/player_manager.hpp"

#include "structure/context.hpp"

void PlayerManager::_parsePlayerIDAssignation(const spk::Message& p_message)
{
	try
	{
		Context::instance()->playerID = p_message.get<ActorMap::ActorID>();
	}
	catch (const std::exception& e)
	{
		PROPAGATE_ERROR("Error while deserializing player ID", e);
	}

	spk::cout << "Receive the player ID " << Context::instance()->playerID << std::endl;
}

PlayerManager::PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::PlayerIDAssignation, [this](const spk::Message& p_message) {
		_parsePlayerIDAssignation(p_message);
	}).relinquish();
}