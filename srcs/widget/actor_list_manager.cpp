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
	requireGeometryUpdate();
	EventDispatcher::emit(Event::PlayerMotion);
}

void ActorListManager::_parseActorRemoval(const spk::Message& p_message)
{
	while (p_message.empty() == false)
	{
		auto actorId = p_message.get<ActorMap::ActorID>();

		Context::instance()->actorMap.removeActor(actorId);
	}
}

void ActorListManager::_onGeometryChange()
{
	for (auto& [shapeID, renderer] : _shapeRenderers)
	{
		renderer->clear();
	}

	for (const auto& [actorID, actor] : Context::instance()->actorMap.actors())
	{
		_shapeRenderers[actor->shape()]->addActor(actor.get());
	}

	for (auto& [shapeID, renderer] : _shapeRenderers)
	{
		renderer->validate();
	}
}

void ActorListManager::_onPaintEvent(spk::PaintEvent& p_event)
{
	for (auto& [shapeID, renderer] : _shapeRenderers)
	{
		renderer->render();
	}
}

void ActorListManager::_onUpdateEvent(spk::UpdateEvent& p_event)
{
	
}

ActorListManager::ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent)
{
	std::vector<Actor::Shape> shapes = {
		Actor::Shape::Triangle,
		Actor::Shape::Square,
		Actor::Shape::Pentagon,
		Actor::Shape::Hexagon,
		Actor::Shape::Octogon,
		Actor::Shape::Circle
	};

	for (const auto& shape : shapes)
	{
		std::unique_ptr<ShapeRenderer> newRenderer = std::make_unique<ShapeRenderer>();

		newRenderer->setShape(shape);

		_shapeRenderers[shape] = std::move(newRenderer);
	}

	Context::instance()->client.subscribe(MessageType::ActorList, [this](const spk::Message& p_message) {
		_parseActorList(p_message);
	}).relinquish();

	Context::instance()->client.subscribe(MessageType::ActorData, [this](const spk::Message& p_message) {
		_parseActorData(p_message);
	}).relinquish();

	Context::instance()->client.subscribe(MessageType::ActorRemoval, [this](const spk::Message& p_message) {
		_parseActorRemoval(p_message);
	}).relinquish();
}