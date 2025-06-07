#pragma once

#include <sparkle.hpp>

#include "network/serializable_object.hpp"

#include <unordered_map>
#include <set>
#include <cstdint>

#include "renderer/chunk_renderer.hpp"

struct Node
{
	enum class Type
	{
		Autotiled,
		Monotiled
	};

    enum class Flag : std::uint16_t
    {
        None         = 0,
        EastBlocked  = 1 << 0,
        WestBlocked  = 1 << 1,
        NorthBlocked = 1 << 2,
        SouthBlocked = 1 << 3,
        Obstacle     = EastBlocked | WestBlocked | NorthBlocked | SouthBlocked
    };

	using Flags = spk::Flags<Flag>;

    Flags flags;
    Type isAutotiled;
    spk::Vector2Int sprite;
	spk::Vector2Int animationOffset;
	float frameDuration;
	float nbFrame;

	Node(Flags p_flags = Flag::None, Type p_isAutotiled = Type::Monotiled, spk::Vector2Int p_sprite = {0, 0}, spk::Vector2Int p_animationOffset = {0, 0}, float p_frameDuration = 0, float p_nbFrame = 1) :
		flags(p_flags),
		isAutotiled(p_isAutotiled),
		sprite(p_sprite),
		animationOffset(p_animationOffset),
		frameDuration(p_frameDuration),
		nbFrame(p_nbFrame)
	{

	}
};

Node::Type toNodeType(const std::wstring& p_string);
Node::Flag toNodeFlag(const std::wstring& p_string);

class NodeMap
{
public:
	using ID = char;

private:
	std::unordered_map<ID, Node> _nodes;

public:
	NodeMap() = default;

	void addNode(const ID& p_id, const Node& p_node);

	bool contains(const ID& p_id) const;

	const Node& node(const ID& p_id) const;
};

class Actor;

class Chunk : public spk::IChunk<NodeMap::ID, 16, 16, 3>, public SerializableObject
{
private:
	spk::Vector2Int _position;
	std::set<spk::SafePointer<Actor>> _bindedActors;
	bool _isBaked = false;
	ChunkRenderer _renderer;
    spk::SafePointer<Chunk> _neighbours[3][3] = {
			{nullptr,nullptr,nullptr},
        	{nullptr,nullptr,nullptr},
        	{nullptr,nullptr,nullptr}
		};

	void _insertAutotile (const Node& p_node, const spk::Vector3Int& p_nodePosition, NodeMap::ID baseIndex);
    void _insertMonotile(const Node& p_node, const spk::Vector3Int& p_nodePosition);

    void _computeNeighbours();
	spk::SafePointer<Chunk> _getNeighbourChunk(const spk::Vector2Int& p_offset) const;
	spk::Vector2Int _getNeighbourOffset(const spk::Vector3Int& p_position) const;

    spk::Vector2Int _computeSpriteOffset(NodeMap::ID baseIndex, int quadrant, const spk::Vector3Int& relPos) const;

public:
	Chunk();

	void setPosition(const spk::Vector2Int& p_position);
	const spk::Vector2Int& position();

	void serialize(spk::Message& p_message) const override;
	void deserialize(const spk::Message& p_message) override;
	static void skip(const spk::Message& p_message);

	bool isBaked() const;
	void unbake();
	void bake();
	void render();

	void bindActor(spk::SafePointer<Actor> p_actor);
	void unbindActor(spk::SafePointer<Actor> p_actor);
	const std::set<spk::SafePointer<Actor>>& bindedActors() const;
};

class Tilemap : public spk::ITilemap<Chunk>
{

public:
	Tilemap();
};