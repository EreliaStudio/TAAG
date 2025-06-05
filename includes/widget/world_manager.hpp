#pragma once

#include <sparkle.hpp>

#include "widget/graphical_widget.hpp"
#include "structure/world.hpp"

class WorldManager : public GraphicalWidget
{
private:
	std::vector<spk::SafePointer<Chunk>> _activeChunks;

	void _onGeometryChange() override;
	void _onPaintEvent(spk::PaintEvent& p_event) override;

	void _requestChunks(const std::vector<Tilemap::ChunkCoordinate>& p_chunkCoordinates);
	void _receiveChunk(const spk::Message& p_message);

public:
	WorldManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};