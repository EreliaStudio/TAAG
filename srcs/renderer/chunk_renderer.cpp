#include "renderer/chunk_renderer.hpp"

void ChunkRenderer::_initProgram()
{
    if (_program == nullptr)
    {
        const char *vertexShaderSrc = R"(#version 450
            layout(location = 0) in vec3 inPosition;
            layout(location = 1) in vec2 inUV;
            
            layout(std140, binding = 0) uniform CameraData
            {
                mat4 view;
                mat4 proj;
            } cameraData;

            layout(location = 0) out vec2 fragUV;

            void main()
            {
                fragUV = inUV;

                gl_Position = cameraData.proj * cameraData.view * vec4(inPosition, 1.0);
            }
            )";

        const char *fragmentShaderSrc = R"(#version 450
            layout(location = 0) in vec2 fragUV;
            layout(location = 0) out vec4 outputColor;

            uniform sampler2D diffuseTexture;

            void main()
            {
                outputColor = texture(diffuseTexture, fragUV);

                if (outputColor.a == 0)
                    discard ;
            }
            )";

        _program = new spk::OpenGL::Program(vertexShaderSrc, fragmentShaderSrc);
    }

	if (_samplerObject == nullptr)
	{
		_samplerObject = std::make_unique<spk::OpenGL::SamplerObject>("diffuseTexture", spk::OpenGL::SamplerObject::Type::Texture2D, 0);
	}
}

void ChunkRenderer::_initBuffers()
{
    _bufferSet = spk::OpenGL::BufferSet({
        {0, spk::OpenGL::LayoutBufferObject::Attribute::Type::Vector3}, // position
        {1, spk::OpenGL::LayoutBufferObject::Attribute::Type::Vector2}, // uv
    });
}

ChunkRenderer::ChunkRenderer() :
	_cameraUBO(UniformBufferObjectAtlas::instance()->ubo(L"cameraData"))
{
    _initProgram();
    _initBuffers();
}

void ChunkRenderer::setSpriteSheet(const spk::SafePointer<spk::SpriteSheet>& p_spriteSheet)
{
	_initProgram();
    _samplerObject->bind(p_spriteSheet);
	_spriteSheet = p_spriteSheet;
}

const spk::SafePointer<spk::SpriteSheet>& ChunkRenderer::spriteSheet()
{
	_initProgram();
	return (_spriteSheet);
}

void ChunkRenderer::clear()
{
    _bufferSet.layout().clear();
    _bufferSet.indexes().clear();
}

void ChunkRenderer::prepare(const spk::Vector3& anchor, const spk::Vector2& size, const spk::Vector2Int& sprite)
{
    if (_spriteSheet == nullptr)
	{
        return;
	}

    const std::size_t nbVertex = _bufferSet.layout().size() / sizeof(Vertex);

    spk::Vector3 bottomLeft = 	{anchor.x,			anchor.y, 			anchor.z};
    spk::Vector3 bottomRight =  {anchor.x + size.x,	anchor.y, 			anchor.z};
    spk::Vector3 topLeft = 		{anchor.x, 			anchor.y + size.y, 	anchor.z};
    spk::Vector3 topRight = 	{anchor.x + size.x, anchor.y + size.y,  anchor.z};

    const auto &section = _spriteSheet->sprite(sprite);

    float u1 = section.anchor.x;
    float v1 = section.anchor.y + section.size.y;
    float u2 = section.anchor.x + section.size.x;
    float v2 = section.anchor.y;

    _bufferSet.layout()
        << Vertex{bottomLeft,  {u1, v1}}
        << Vertex{bottomRight, {u2, v1}}
        << Vertex{topLeft,     {u1, v2}}
        << Vertex{topRight,    {u2, v2}};

    static constexpr std::array<unsigned int, 6> kIndices = {0, 1, 2, 2, 1, 3};
    for (unsigned int idx : kIndices)
	{
        _bufferSet.indexes() << idx + static_cast<unsigned int>(nbVertex);
	}
}

void ChunkRenderer::validate()
{
    _bufferSet.layout().validate();
    _bufferSet.indexes().validate();
}

void ChunkRenderer::render()
{
    if (_spriteSheet == nullptr)
	{
		return;
	} 

    _program->activate();
    _bufferSet.activate();
    _samplerObject->activate();
	_cameraUBO.activate();

    _program->render(_bufferSet.indexes().nbIndexes(), 1);

	_cameraUBO.deactivate();
    _samplerObject->deactivate();
    _bufferSet.deactivate();
    _program->deactivate();
}