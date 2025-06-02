#include "network/network_override.hpp"

std::string toString(MessageType p_type)
{
    switch (p_type)
    {
        case MessageType::ChunkRequest:        return "ChunkRequest";
        case MessageType::ChunkData:           return "ChunkData";
        case MessageType::ActorRequest:        return "ActorRequest";
        case MessageType::ActorData:           return "ActorData";
        case MessageType::ActorList:           return "ActorList";
        case MessageType::PlayerIDAssignation: return "PlayerIDAssignation";
        default:                               return "Unknown";
    }
}

std::wstring toWString(MessageType p_type)
{
    switch (p_type)
    {
        case MessageType::ChunkRequest:        return L"ChunkRequest";
        case MessageType::ChunkData:           return L"ChunkData";
        case MessageType::ActorRequest:        return L"ActorRequest";
        case MessageType::ActorData:           return L"ActorData";
        case MessageType::ActorList:           return L"ActorList";
        case MessageType::PlayerIDAssignation: return L"PlayerIDAssignation";
        default:                               return L"Unknown";
    }
}

Client::Client() : spk::Client()
{
	addOnDisconnectionCallback([](){
		EventDispatcher::emit(Event::LaunchClientConnection);
	}).relinquish();
}

Client::MessageContract Client::subscribe(MessageType p_type, const MessageJob& p_job)
{
	return _messageHandlers[p_type].subscribe(p_job);
}

void Client::update()
{
	while (messages().empty() == false)
	{
		spk::Client::MessageObject message = messages().pop();

		auto it = _messageHandlers.find(static_cast<MessageType>(message->header().type));
		if (it != _messageHandlers.end())
		{
			it->second.trigger(*message);
		}
		else
		{
			spk::cout << L"Unhandled message type: " << toWString(static_cast<MessageType>(message->header().type)) << std::endl;
		}
	}
}