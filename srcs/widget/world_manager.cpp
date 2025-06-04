#include "widget/world_manager.hpp"

#include "structure/context.hpp"

void WorldManager::_onGeometryChange()
{
	spk::Vector2Int topLeftCorner = Tilemap::worldToChunk(spk::Vector3Int(convertScreenToWorldPosition({0, 0}), 0));
	spk::Vector2Int downRightCorner = Tilemap::worldToChunk(spk::Vector3Int(convertScreenToWorldPosition(geometry().size), 0));
	
	std::vector<Tilemap::ChunkCoordinates> chunkToRequest;

	for (int x = topLeftCorner.x; x <= downRightCorner; x++)
	{
		for (int y = topLeftCorner.y; y <= downRightCorner; y++)
		{
			Tilemap::ChunkCoordinates tmp = {x, y};

			if (Context::instance()->tilemap.contains(tmp) == false)
			{
				chunkToRequest.push_back(tmp);
			}
			else
			{
				spk::SafePointer<Chunk> tmpChunk = Context::instance()->tilemap.chunk(tmp);

				if (tmpChunk->isBaked() == false)
				{
					_chunkRenderer.bake();
				}
			}
		}
	}

	if (chunkToRequest.empty() == false)
	{
		_requestChunks(chunkToRequest);
	}
}

void WorldManager::_onPaintEvent(spk::PaintEvent& p_event)
{
	_chunkRenderer.render();
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

		bool alreadyExist = Context::instance()->tilemap.contains(coordinate);
		spk::SafePointer<Chunk> chunk = Context::instance()->tilemap.requestChunk(coordinate);

		if (alreadyExist == false)
		{
			chunk->setPosition(coordinate);
		}

		try
		{
			chunk->deserialize(p_message);
		}
		catch (const std::exception& e)
		{
			PROPAGATE_ERROR("Error while deserializing chunk", e);
		}
	}
	requireGeometryUpdate();
}

WorldManager::WorldManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::ChunkData, [this](const spk::Message& p_message) {
		_receiveChunk(p_message);
	}).relinquish();
}