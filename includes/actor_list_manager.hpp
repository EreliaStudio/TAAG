#pragma once

#include <sparkle.hpp>

class ActorListManager : public spk::Widget
{
private:
	void _requestActorList(const std::vector<spk::ActorMap::ID>& p_actorIDs);
	void _parseActorList(const spk::Message& p_message);

public:
	ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};