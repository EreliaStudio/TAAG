#include "actor_list_manager.hpp"

#include "context.hpp"

void ActorListManager::_requestActorList(const std::vector<spk::ActorMap::ID>& p_actorIDs)
{
	if (missingActorIDs.empty() == false)
	{
		spk::Message actorRequestMessage = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::ActorListRequest));
		
		for (auto& actorId : missingActorIDs)
		{
			actorRequestMessage << actorId;
		}

		Context::instance()->client.send(actorRequestMessage);
	}
}

void ActorListManager::_parseActorList(const spk::Message& p_message)
{
	std::vector<ActorMap::ID> missingActorIDs;

	while (p_message.empty() == false)
	{
		auto actorId = p_message.read<long long>();

		if (Context::instance()->actorMap.contains(actorId) == false)
		{
			missingActorIDs.push_back(actorId);

			p_message.skip<spk::Vector2>();
			p_message.skip<float>();
		}
		else
		{
			auto actorPosition = p_message.read<spk::Vector2>();
			auto actorRotation = p_message.read<float>();

			auto actor = Context::instance()->actorMap.actor(actorId);
			if (actor != nullptr)
			{
				actor->setPosition(actorPosition);
				actor->setRotation(actorRotation);
			}
		}
	}

	_requestActorList(missingActorIDs);
}

ActorListManager::ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::ActorList, [this](const spk::Message& p_message) {
		_parseActorList(p_message);
	}).relinquish();
}