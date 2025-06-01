#pragma once
#pragma once

#include <sparkle.hpp>

struct Node
{
	bool isObstacle = false;
};

class NodeMap
{
public:
	using ID = char;

private:
	std::unordered_map<ID, Node> _nodes;

public:
	NodeMap() = default;

	void addNode(const ID& p_id, Node p_node);

	bool contains(const ID& p_id) const;

	const Node& node(const ID& p_id) const;
};

class Actor;

class Chunk : public spk::IChunk<NodeMap::ID, 16, 16, 3>
{
private:

public:
	Chunk();

	void bindActor(spk::SafePointer<Actor> p_actor);
	void unbindActor(spk::SafePointer<Actor> p_actor);
};

class Tilemap : public spk::ITilemap<Chunk>
{
private:
	spk::Perlin _perlinGenerator;

	void _generateChunk(spk::SafePointer<Chunk>& p_chunk);

	void _onChunkGeneration(const ChunkCoordinate& p_coordinates, spk::SafePointer<Chunk> p_chunk);

public:
	Tilemap();
};