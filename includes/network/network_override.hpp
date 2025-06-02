#pragma once

#include <sparkle.hpp>

#include "event.hpp"

enum class MessageType
{
	ChunkRequest = 0,
	ChunkData = 1,
	ActorRequest = 2,
	ActorData = 3,
	ActorList = 4,
	PlayerIDAssignation = 5
};

std::string toString(MessageType p_type);
std::wstring toWString(MessageType p_type);

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