#pragma once

#include <sparkle.hpp>

class Client : public spk::Singleton<Client>
{
	friend class spk::Singleton<Client>;
private:
	std::string _address;
	const size_t _port = 26500;
	spk::Client _client;

	Client() = default;

public:
	spk::Client::Contract addOnConnectionCallback(const spk::Client::ConnectionCallback &p_connectionCallback);
	spk::Client::Contract addOnDisconnectionCallback(const spk::Client::DisconnectionCallback &p_disconnectionCallback);

	bool isConnected()
	{
		return (_client.isConnected());
	}
	void setup(const std::string &p_address)
	{
		_address = p_address;
	}
	void connect()
	{
		_client.connect(_address, _port);
	}
	void disconnect()
	{
		_client.disconnect();
	}
	void send(const spk::Message &p_message)
	{
		_client.send(p_message);
	}
	spk::ThreadSafeQueue<spk::Client::MessageObject> &messages()
	{
		return (_client.messages());
	}
};

class Server : public spk::Singleton<Server>
{
	friend class spk::Singleton<Server>;

private:
	const size_t _port = 26500;
	spk::Server _server;

	Server() = default;

public:
	void start()
	{
		_server.start(_port);
	}

	void stop()
	{
		_server.stop();
	}

	spk::Server::Contract addOnConnectionCallback(const spk::Server::ConnectionCallback &p_connectionCallback)
	{
		return std::move(_server.addOnConnectionCallback(p_connectionCallback));
	}
	
	spk::Server::Contract addOnDisconnectionCallback(const spk::Server::DisconnectionCallback &p_disconnectionCallback)
	{
		return std::move(_server.addOnDisconnectionCallback(p_disconnectionCallback));
	}

	void sendTo(spk::Server::ClientID p_clientID, const spk::Message& p_msg)
	{
		_server.sendTo(p_clientID, p_msg);
	}

	void sendTo(const std::vector<spk::Server::ClientID> &p_clients, const spk::Message &p_message)
	{
		_server.sendTo(p_clients, p_message);
	}

	void sendToAll(const spk::Message& p_msg)
	{
		_server.sendToAll(p_msg);
	}

	spk::ThreadSafeQueue<spk::Server::MessageObject> &messages()
	{
		return _server.messages();
	}
};