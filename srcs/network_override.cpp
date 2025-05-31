#include "network_override.hpp"

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
			spk::cout << L"Unhandled message type: " << static_cast<int>(message->header().type) << std::endl;
		}
	}
}