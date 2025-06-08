#pragma once

#include <sparkle.hpp>

#include "structure/actor.hpp"

class ShapeRenderer
{
private:
	UniformBufferObjectAtlas::Instanciator _uniformBufferObjectAtlasInstanciator;

	struct Vertex
	{
		spk::Vector3 position;
	};

	struct DynamicElement
	{
		spk::Vector2 position;
		float rotation;
		SPK_PADDING(1);
		spk::Vector2 scale;
		SPK_PADDING(2);
		spk::Color color;
	};

	static inline spk::OpenGL::Program *_program = nullptr;

	spk::OpenGL::BufferSet _bufferSet;
	spk::OpenGL::ShaderStorageBufferObject _actorSSBO;
	spk::OpenGL::UniformBufferObject& _cameraUBO;

	static void _initProgram();
	void _initBuffers();

	std::vector<spk::Vector3> _generatePoints(const Actor::Shape& p_shape);

public:
	ShapeRenderer();

	void setShape(const Actor::Shape& p_shape);
	
	void clear();
	void addActor(spk::SafePointer<Actor> p_actor);
	void validate();

	void render();
};