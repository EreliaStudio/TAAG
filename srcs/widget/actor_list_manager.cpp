#include "widget/actor_list_manager.hpp"

#include "structure/context.hpp"

void ActorListManager::_requestActorList(const std::vector<ActorMap::ActorID>& p_actorIDs)
{
	if (p_actorIDs.empty() == false)
	{
		spk::Message actorRequestMessage = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::ActorRequest));
		
		for (auto& actorId : p_actorIDs)
		{
			actorRequestMessage << actorId;
		}

		Context::instance()->client.send(actorRequestMessage);
	}
}

void ActorListManager::_parseActorData(const spk::Message& p_message)
{
	while (p_message.empty() == false)
	{
		auto actorId = p_message.get<ActorMap::ActorID>();

		try
		{
			Context::instance()->actorMap.requestActor(actorId)->deserialize(p_message);
		}
		catch (const std::exception& e)
		{
			PROPAGATE_ERROR("Error while deserializing actor data", e);
		}
	}
}

void ActorListManager::_parseActorList(const spk::Message& p_message)
{
	std::vector<ActorMap::ActorID> missingActorIDs;

	while (p_message.empty() == false)
	{
		auto actorId = p_message.get<ActorMap::ActorID>();

		if (Context::instance()->actorMap.contains(actorId) == false)
		{
			missingActorIDs.push_back(actorId);

			Actor::skip(p_message);
		}
		else
		{
			auto actor = Context::instance()->actorMap.actor(actorId);

			try
			{
				actor->deserialize(p_message);
			}
			catch (const std::exception& e)
			{
				PROPAGATE_ERROR("Error while deserializing actor", e);
			}
		}
	}

	_requestActorList(missingActorIDs);
	requestPaint();
}

ActorListManager::ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::ActorList, [this](const spk::Message& p_message) {
		_parseActorList(p_message);
	}).relinquish();

	Context::instance()->client.subscribe(MessageType::ActorData, [this](const spk::Message& p_message) {
		_parseActorData(p_message);
	}).relinquish();
}