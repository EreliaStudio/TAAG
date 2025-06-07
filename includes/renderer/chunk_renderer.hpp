#pragma once

#include <sparkle.hpp>

#include "utils/uniform_buffer_object_atlas.hpp"

class ChunkRenderer
{
private:
	UniformBufferObjectAtlas::Instanciator _uniformBufferObjectAtlasInstanciator;

	struct Vertex
	{
		spk::Vector3 position;
		spk::Vector2 uv;
	};

	static inline spk::OpenGL::Program *_program = nullptr;
	static inline std::unique_ptr<spk::OpenGL::SamplerObject> _samplerObject = nullptr;
	static inline spk::SafePointer<spk::SpriteSheet> _spriteSheet = nullptr;

	spk::OpenGL::BufferSet _bufferSet;
	spk::OpenGL::UniformBufferObject& _cameraUBO;

	static void _initProgram();
	void _initBuffers();

public:
	ChunkRenderer();

	static void setSpriteSheet(const spk::SafePointer<spk::SpriteSheet>& p_spriteSheet);
	static const spk::SafePointer<spk::SpriteSheet>& spriteSheet();

	void clear();
	void prepare(const spk::Vector3& anchor, const spk::Vector2& size, const spk::Vector2Int& sprite);
	void validate();

	void render();
};