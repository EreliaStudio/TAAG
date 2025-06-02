#include "widget/world_manager.hpp"

#include "structure/context.hpp"

void WorldManager::_onGeometryChange()
{

}

void WorldManager::_onPaintEvent(spk::PaintEvent& p_event)
{

}

void WorldManager::_requestChunks(const std::vector<Tilemap::ChunkCoordinate>& p_chunkCoordinates)
{
	spk::Message message = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::ChunkRequest));

	for (const auto& coordinate : p_chunkCoordinates)
	{
		message << coordinate;
	}

	Context::instance()->client.send(message);
}

void WorldManager::_receiveChunk(const spk::Message& p_message)
{
	while (p_message.empty() == false)
	{
		Tilemap::ChunkCoordinate coordinate;
		p_message >> coordinate;

		spk::SafePointer<Chunk> chunk = Context::instance()->tilemap.requestChunk(coordinate);

		try
		{
			chunk->deserialize(p_message);
		}
		catch (const std::exception& e)
		{
			PROPAGATE_ERROR("Error while deserializing chunk", e);
		}
	}
}

WorldManager::WorldManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	spk::Widget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::ChunkData, [this](const spk::Message& p_message) {
		_receiveChunk(p_message);
	}).relinquish();
}