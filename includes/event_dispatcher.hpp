#pragma once

#include <sparkle.hpp>

#include "event.hpp"

class EventDispatcher : public spk::Singleton<EventDispatcher>
{
	friend class spk::Singleton<EventDispatcher>;
public:
	using Observer = spk::Observer<Event>;
	using Contract = Observer::Contract;
	using Job = Observer::Job;

private:
	Observer _observer;

	EventDispatcher() = default;

public:
	void emit(Event p_event)
	{
		_observer.notifyEvent(p_event);
	}

	void invalidateContracts(Event p_event)
	{
		_observer.invalidateContracts(p_event);
	}

	Observer::Contract subscribe(Event p_event, const Job &p_job)
	{
		return (std::move(_observer.subscribe(p_event, p_job)));
	}
};
