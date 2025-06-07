#pragma once

#include <sparkle.hpp>

class UniformBufferObjectAtlas : public spk::Singleton<UniformBufferObjectAtlas>
{
	friend class spk::Singleton<UniformBufferObjectAtlas>;

private:
	std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject> _ubos;

	void _loadElement(spk::OpenGL::UniformBufferObject &p_ubo, const spk::JSON::Object &p_elementDesc);
	void _loadElement(spk::DataBufferLayout::Element &p_parent, const spk::JSON::Object &p_elementDesc);

public:
	UniformBufferObjectAtlas();

	void load(const spk::JSON::File &p_json);
	bool contains(const std::wstring &p_name) const;
	spk::OpenGL::UniformBufferObject& ubo(const std::wstring &p_name);
	const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject> &ubos() const;
};