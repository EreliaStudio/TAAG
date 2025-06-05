#pragma once

#include <sparkle.hpp>

#include "network/serializable_object.hpp"
#include "utils/bitmask.hpp"

#include <unordered_map>
#include <set>
#include <cstdint>

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
	size_t animationFrames;
	spk::Vector2Int animationOffset;

	Node(Flags p_flags = Flag::None, Type p_isAutotiled = Type::Monotiled, spk::Vector2Int p_sprite = {0, 0}, size_t p_animationFrames = 1, spk::Vector2Int p_animationOffset = {1, 0}) :
		flags(p_flags),
		isAutotiled(p_isAutotiled),
		sprite(p_sprite),
		animationFrames(p_animationFrames),
		animationOffset(p_animationOffset)
	{

	}
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
public:
	static void setSpriteSheet(spk::SafePointer<spk::SpriteSheet> p_spriteSheet);

private:
	spk::Vector2Int _position;
	std::set<spk::SafePointer<Actor>> _bindedActors;
	bool _isBaked = false;
	spk::TextureRenderer _renderer;

public:
	Chunk();

	void setPosition(const spk::Vector2Int& p_position);
	const spk::Vector2Int& position();

	void serialize(spk::Message& p_message) const override;
	void deserialize(const spk::Message& p_message) override;
	static void skip(const spk::Message& p_message);

	bool isBaked() const;
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