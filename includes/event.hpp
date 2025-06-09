#pragma once

#include <sparkle.hpp>

enum class Event
{
	LaunchClientConnection,
	GameReady,
	PlayerMotion
};

using EventDispatcher = spk::EventDispatcher<Event>;