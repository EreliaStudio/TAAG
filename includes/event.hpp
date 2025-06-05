#pragma once

#include <sparkle.hpp>

enum class Event
{
	LaunchClientConnection,
	GameReady
};

using EventDispatcher = spk::EventDispatcher<Event>;