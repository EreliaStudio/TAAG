#pragma once

class Actor
{
private:
	spk::Vector2 _position;
	float rotation;

public:
	Actor()
	{

	}

	void setRotation(float angle)
	{
		rotation = angle;
	}

	float rotation() const
	{
		return (rotation);
	}

	void setPosition(const spk::Vector2& position)
	{
		_position = position;
	}

	const spk::Vector2& position() const
	{
		return (_position);
	}
};

class ActorMap
{
public:
	using ID = long long;

private:
	std::unordered_map<ID, std::unique_ptr<Actor>> _actors;

public:
	void addActor(ID id, std::unique_ptr<Actor> actor)
	{
		_actors[id] = std::move(actor);
	}

	bool contains(ID id) const
	{
		return _actors.find(id) != _actors.end();
	}

	spk::SafePointer<Actor> actor(ID id) const
	{
		return _actors.at(id).get();
	}

	void removeActor(ID id)
	{
		_actors.erase(id);
	}
};