#include <sparkle.hpp>

class Client
{
private:
	static inline std::string _address;
	static inline const size_t _port = 26500;
	static inline spk::Client _client;

public:
	spk::Client::Contract addOnConnectionCallback(const spk::Client::ConnectionCallback &p_connectionCallback);
	spk::Client::Contract addOnDisconnectionCallback(const spk::Client::DisconnectionCallback &p_disconnectionCallback);

	static bool isConnected()
	{
		return (_client.isConnected());
	}
	static void setup(const std::string &p_address)
	{
		_address = p_address;
	}
	static void connect()
	{
		_client.connect(_address, _port);
	}
	static void disconnect()
	{
		_client.disconnect();
	}
	static void send(const spk::Message &p_message)
	{
		_client.send(p_message);
	}
	static spk::ThreadSafeQueue<spk::Client::MessageObject> &messages()
	{
		return (_client.messages());
	}
};

class Server
{
private:
	static inline const size_t _port = 26500;
	static inline spk::Server _server;

public:
	static void start()
	{
		_server.start(_port);
	}

	static void stop()
	{
		_server.stop();
	}

	static spk::Server::Contract addOnConnectionCallback(const spk::Server::ConnectionCallback &p_connectionCallback)
	{
		return std::move(_server.addOnConnectionCallback(p_connectionCallback));
	}
	
	static spk::Server::Contract addOnDisconnectionCallback(const spk::Server::DisconnectionCallback &p_disconnectionCallback)
	{
		return std::move(_server.addOnDisconnectionCallback(p_disconnectionCallback));
	}

	static void sendTo(spk::Server::ClientID p_clientID, const spk::Message& p_msg)
	{
		_server.sendTo(p_clientID, p_msg);
	}

	static void sendTo(const std::vector<spk::Server::ClientID> &p_clients, const spk::Message &p_message)
	{
		_server.sendTo(p_clients, p_message);
	}

	static void sendToAll(const spk::Message& p_msg)
	{
		_server.sendToAll(p_msg);
	}

	static spk::ThreadSafeQueue<spk::Server::MessageObject> &messages()
	{
		return _server.messages();
	}
};

enum class Event
{
	EnterMenuHUD,
	EnterGameHUD
};

class EventDispatcher
{
public:
	using Observer = spk::Observer<Event>;
	using Contract = Observer::Contract;
	using Job = Observer::Job;

private:
	static inline Observer _observer;

public:
	static void emit(Event p_event)
	{
		_observer.notifyEvent(p_event);
	}

	static void invalidateContracts(Event p_event)
	{
		_observer.invalidateContracts(p_event);
	}

	static Observer::Contract subscribe(Event p_event, const Job &p_job)
	{
		return (std::move(_observer.subscribe(p_event, p_job)));
	}
};

class AssetAtlas
{
private:
	static inline std::unordered_map<std::wstring, spk::SafePointer<spk::Image>> _images;
	static inline std::unordered_map<std::wstring, spk::SafePointer<spk::SpriteSheet>> _spriteSheets;
	static inline std::unordered_map<std::wstring, spk::SafePointer<spk::Font>> _fonts;

public:
	static void load(const spk::JSON::File& p_atlasFileConfiguration)
	{
		
	}

	static spk::SafePointer<spk::Image> image(const std::wstring& p_name)
	{
		if (_images.contains(p_name) == false)
		{
			throw std::runtime_error("Image not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return _images[p_name];
	}

	static spk::SafePointer<spk::SpriteSheet> spriteSheet(const std::wstring& p_name)
	{
		if (_spriteSheets.contains(p_name) == false)
		{
			throw std::runtime_error("SpriteSheet not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return _spriteSheets[p_name];
	} 

	static spk::SafePointer<spk::Font> font(const std::wstring& p_name)
	{
		if (_fonts.contains(p_name) == false)
		{
			throw std::runtime_error("Font not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return _fonts[p_name];
	}
};

class PushButton : public spk::PushButton
{
private:

public:
	PushButton(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::PushButton(p_name, p_parent)
	{
		
	}
};

class TextLabel : public spk::TextLabel
{
private:

public:
	TextLabel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextLabel(p_name, p_parent)
	{
		
	}
};

class TextEdit : public spk::TextEdit
{
private:

public:
	TextEdit(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextEdit(p_name, p_parent)
	{
		
	}
};

class GameHUD : public spk::Screen
{
private:

public:
	GameHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{
		
	}
};

class MenuHUD : public spk::Screen
{
private:

public:
	MenuHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{

	}
};

class MainWidget : public spk::Widget
{
private:
	GameHUD _gameHUD;
	EventDispatcher::Contract _gameHUDContract;
	MenuHUD _menuHUD;
	EventDispatcher::Contract _menuHUDContract;

	void _onGeometryChange() override
	{
		_gameHUD.setGeometry(geometry());
		_menuHUD.setGeometry(geometry());
	}

public:
	MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent),
		_gameHUD(p_name + L"/GameHUD", this),
		_menuHUD(p_name + L"/MenuHUD", this)
	{
		_gameHUDContract = EventDispatcher::subscribe(Event::EnterGameHUD, [&]() {
			_gameHUD.activate();
		});

		_menuHUDContract = EventDispatcher::subscribe(Event::EnterMenuHUD, [&]() {
			_menuHUD.activate();
		});
	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {800, 600});

	MainWidget mainWidget(L"MainWidget", win);
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::emit(Event::EnterMenuHUD);

	return (app.run());
}