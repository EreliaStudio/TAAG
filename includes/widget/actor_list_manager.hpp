#pragma once

#include <sparkle.hpp>

#include "structure/actor_map.hpp"

class ActorListManager : public spk::Widget
{
private:
	void _requestActorList(const std::vector<ActorMap::ActorID>& p_actorIDs);
	void _parseActorData(const spk::Message& p_message);
	void _parseActorList(const spk::Message& p_message);

public:
	ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};