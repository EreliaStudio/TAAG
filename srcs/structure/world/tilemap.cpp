#include "structure/world.hpp"

Tilemap::Tilemap() :
	spk::ITilemap<Chunk>()
{
	requestChunk({0, 0});
}