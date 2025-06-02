#pragma once
#pragma once

#include <sparkle.hpp>

#include "network/serializable_object.hpp"

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

class Chunk : public spk::IChunk<NodeMap::ID, 16, 16, 3>, public SerializableObject
{
private:
	std::set<spk::SafePointer<Actor>> _bindedActors;

public:
	Chunk();

	void serialize(spk::Message& p_message) const override;
	void deserialize(const spk::Message& p_message) override;
	static void skip(const spk::Message& p_message);

	void bindActor(spk::SafePointer<Actor> p_actor);
	void unbindActor(spk::SafePointer<Actor> p_actor);
	const std::set<spk::SafePointer<Actor>>& bindedActors() const;
};

class Tilemap : public spk::ITilemap<Chunk>
{

public:
	Tilemap();
};