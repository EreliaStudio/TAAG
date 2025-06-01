#pragma once

#include <sparkle.hpp>

#include "event.hpp"

enum class MessageType
{
	ChunkRequest = 0,
	ChunkAwnser = 1,
	ChunkUpdate = 2,
	ActorRequest = 3,
	ActorAwnser = 4,
	ActorUpdate = 5,
	PlayerIDAssignation = 6
};

class Client : public spk::Client
{
private:
	using MessageContractProvider = spk::TContractProvider<const spk::Message&>;
	using MessageContract = MessageContractProvider::Contract;
	using MessageJob = MessageContractProvider::Job;

private:
	std::unordered_map<MessageType, MessageContractProvider> _messageHandlers;

public:
	Client();

	MessageContract subscribe(MessageType p_type, const MessageJob& p_job);

	void update();
};