#pragma once

#include <sparkle.hpp>

#include 

struct Context : public spk::Singleton<Context>
{
private:
	AssetAtlas::Instanciator _assetAtlasInstanciator;

	Context() = default;

public:
	enum class Mode
	{
		None,
		Host,
		Client
	};

	Mode mode = Mode::None;
	std::string _hostAddress = "localhost";
	size_t _hostPort = 26500;
};