#include "widget/world_manager.hpp"

#include "structure/context.hpp"

void WorldManager::_onGeometryChange()
{
	spk::Vector2 topLeftCell = convertScreenToWorldPosition({0, 0});
	spk::Vector2 downRightCell = convertScreenToWorldPosition(geometry().size);
	spk::Vector2Int topLeftCorner = Tilemap::worldToChunk(spk::Vector3Int(topLeftCell, 0)) - 1;
	spk::Vector2Int downRightCorner = Tilemap::worldToChunk(spk::Vector3Int(downRightCell, 0)) + 1;
	
	std::vector<Tilemap::ChunkCoordinate> chunkToRequest;

	_activeChunks.clear();

	for (int x = topLeftCorner.x; x <= downRightCorner.x; x++)
	{
		for (int y = topLeftCorner.y; y <= downRightCorner.y; y++)
		{
			Tilemap::ChunkCoordinate tmp = {x, y};

			if (Context::instance()->tilemap.contains(tmp) == false)
			{
				chunkToRequest.push_back(tmp);
			}
			else
			{
				spk::SafePointer<Chunk> tmpChunk = Context::instance()->tilemap.chunk(tmp);

				if (tmpChunk->isBaked() == false)
				{
					tmpChunk->bake();
				}

				_activeChunks.push_back(tmpChunk);
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
	for (auto& chunk : _activeChunks)
	{
		chunk->render();
	}
}

void WorldManager::_requestChunks(const std::vector<Tilemap::ChunkCoordinate>& p_ChunkCoordinate)
{
	spk::Message message = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::ChunkRequest));

	for (const auto& coordinate : p_ChunkCoordinate)
	{
		message << coordinate;
	}

	Context::instance()->client.send(message);
}

void WorldManager::_unbakeNeighbourChunks(const Tilemap::ChunkCoordinate& center)
{
	Tilemap& tilemap = Context::instance()->tilemap;

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			if (dx == 0 && dy == 0)
			{
				continue;
			}

			Tilemap::ChunkCoordinate neighbour{center.x + dx, center.y + dy};

			if (tilemap.contains(neighbour))
			{
				auto chunk = tilemap.chunk(neighbour);
				if (chunk && chunk->isBaked() == true)
				{
					chunk->unbake();
				}
			}
		}
	}
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
		
		_unbakeNeighbourChunks(coordinate);
	}
	requireGeometryUpdate();
}

WorldManager::WorldManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent)
{
	Context::instance()->client.subscribe(MessageType::ChunkData, [this](const spk::Message& p_message) {
		_receiveChunk(p_message);
	}).relinquish();

	EventDispatcher::subscribe(Event::PlayerMotion, [&](){
		requireGeometryUpdate();
	}).relinquish();
}