#pragma once

#include "network_override.hpp"

struct Context : public spk::Singleton<Context>
{
	Client client;
};