#include "player_manager.hpp"

#include "context.hpp"

void _parsePlayerIDAssignation(const spk::Message& p_message)
{
	auto playerID = p_message.read<ActorMap::ID>();
	Context::instance()->playerID = playerID;
}

PlayerManager::PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::PlayerIDAssignation, [this](const spk::Message& p_message) {
		_parsePlayerIDAssignation(p_message);
	}).relinquish();
}