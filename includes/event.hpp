#pragma once

#include <sparkle.hpp>

enum class Event
{
	LaunchClientConnection,
	ClientConnected
};

using EventDispatcher = spk::EventDispatcher<Event>;