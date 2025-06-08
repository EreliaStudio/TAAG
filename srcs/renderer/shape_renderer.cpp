#include "renderer/shape_renderer.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

void ShapeRenderer::_initProgram()
{
	if (_program == nullptr)
    {
        const char *vertexShaderSrc = R"(#version 450
            layout(location = 0) in vec3 inPosition;
            
            layout(std140, binding = 0) uniform CameraData
            {
                mat4 view;
                mat4 proj;
            } cameraData;

			struct Actor
			{
				vec2 position;
				float rotation;
				vec2 scale;
				vec4 color;
			};

			layout(std430, binding = 1) buffer ActorList
			{
				Actor[] datas;
			} actorList;

            layout(location = 0) out vec4 fragColor;

			vec3 applyRotationAroundZAxis(vec3 basePoint, float rotation)
			{
				float c = cos(rotation);
				float s = sin(rotation);
				return vec3(
					basePoint.x * c - basePoint.y * s,
					basePoint.x * s + basePoint.y * c,
					basePoint.z
				);
			}

            void main()
            {
				Actor actor = actorList.datas[gl_InstanceID];
				
				fragColor = actor.color;

				vec3 scaled    = inPosition * vec3(actor.scale, 1.0);
    			vec3 rotated   = applyRotationAroundZAxis(scaled, actor.rotation);
    			vec3 translated = rotated + vec3(actor.position, 0.0);

				vec4 worldPos = vec4(translated, 1.0);

				gl_Position = cameraData.proj * cameraData.view * worldPos;
            }
            )";

        const char *fragmentShaderSrc = R"(#version 450
            layout(location = 0) in vec4 fragColor;
            layout(location = 0) out vec4 outputColor;

            void main()
            {
                outputColor = fragColor;

                if (outputColor.a == 0)
                    discard ;
            }
            )";

        _program = new spk::OpenGL::Program(vertexShaderSrc, fragmentShaderSrc);
    }
}

void ShapeRenderer::_initBuffers()
{
	_bufferSet = spk::OpenGL::BufferSet({
        {0, spk::OpenGL::LayoutBufferObject::Attribute::Type::Vector3}, // position
    });

}

ShapeRenderer::ShapeRenderer() :
	_actorSSBO(UniformBufferObjectAtlas::instance()->ssbo(L"actorList")),
	_cameraUBO(UniformBufferObjectAtlas::instance()->ubo(L"cameraData"))
{
    _initProgram();
    _initBuffers();
}

std::vector<spk::Vector3> ShapeRenderer::_generatePoints(const Actor::Shape& p_shape)
{
	size_t sides = static_cast<size_t>(p_shape);

    std::vector<spk::Vector3> vertices;
    vertices.reserve(sides);

    for (size_t i = 0; i < sides; ++i)
    {
        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(sides);
        vertices.emplace_back(std::cos(angle) * 0.5f, std::sin(angle) * 0.5f, 0.0f);
    }

    return vertices;
}

void ShapeRenderer::setShape(const Actor::Shape& p_shape)
{
	_bufferSet.clear();

    const std::vector<spk::Vector3> perimeter = _generatePoints(p_shape);

    const size_t baseVertex = _bufferSet.layout().size() / sizeof(spk::Vector3);

    const spk::Vector3 centre(0.0f, 0.0f, 0.0f);

    _bufferSet.layout() << centre;

    for (const spk::Vector3& v : perimeter)
	{
        _bufferSet.layout() << Vertex{v};
	}

    const unsigned int centerIdx = static_cast<unsigned int>(baseVertex);
    const unsigned int firstPerimIdx = centerIdx + 1;
    const unsigned int perimCount = static_cast<unsigned int>(perimeter.size());

    for (unsigned int i = 0; i < perimCount; ++i)
    {
        const unsigned int idx2 = firstPerimIdx + i;
        const unsigned int idx3 = firstPerimIdx + (i + 1) % perimCount;
        _bufferSet.indexes() << centerIdx << idx2 << idx3;
    }

    _bufferSet.validate();
}

void ShapeRenderer::clear()
{
	_actorSSBO.clear();
}

void ShapeRenderer::addActor(spk::SafePointer<Actor> p_actor)
{
	if (p_actor == nullptr)
	{
		return;
	}

	DynamicElement gpuElem;
	gpuElem.position = p_actor->position();
	gpuElem.rotation = p_actor->rotation();
	gpuElem.scale    = p_actor->scale();
	gpuElem.color    = p_actor->color();

	_actorSSBO.dynamicArray().push_back(gpuElem);
}

void ShapeRenderer::validate()
{
	_actorSSBO.validate();
}

void ShapeRenderer::render()
{
	if (_actorSSBO.dynamicArray().nbElement() == 0)
		return ;

	_program->activate();

	_bufferSet.activate();
	_actorSSBO.activate();
	_cameraUBO.activate();
	
    _program->render(_bufferSet.indexes().nbIndexes(), _actorSSBO.dynamicArray().nbElement());

	_cameraUBO.activate();
	_actorSSBO.activate();
	_bufferSet.activate();

}