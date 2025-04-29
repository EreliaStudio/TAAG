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
	CloseApplication,
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
	/*
	{
		"Image":[
			{
				"Name":"Image",
				"Path":"assets/images/image.png"
			}
		],
		"SpriteSheet":[
			{
				"Name":"SpriteSheet",
				"Path":"assets/spritesheets/spritesheet.png",
				"Size":{
					"x":64,
					"y":64
				}
			}
		],
		"Font":[
			{
				"Name":"Font",
				"Path":"assets/fonts/font.ttf"
			}
		]
	}
	 */
	static inline std::unordered_map<std::wstring, spk::Image> _images;
	static inline std::unordered_map<std::wstring, spk::SpriteSheet> _spriteSheets;
	static inline std::unordered_map<std::wstring, spk::Font> _fonts;

public:
	static void load(const spk::JSON::File& p_atlasFileConfiguration)
	{
		{
			const auto& imagesNode = p_atlasFileConfiguration[L"images"];

			if (!imagesNode.isArray())
				GENERATE_ERROR("The \"images\" entry must be a JSON array");

			for (const spk::JSON::Object* imagePtr : imagesNode.asArray())
			{
				const auto& image = *imagePtr;

				const std::wstring& name = image[L"name"].as<std::wstring>();
				const std::wstring& path = image[L"path"].as<std::wstring>();

				_images[name] = spk::Image(path);
			}
		}

		/****************  SPRITE SHEETS  ***************/
		{
			const auto& sheetsNode = p_atlasFileConfiguration[L"spriteSheets"];

			if (!sheetsNode.isArray())
				GENERATE_ERROR("The \"spriteSheets\" entry must be a JSON array");

			for (const spk::JSON::Object* sheetPtr : sheetsNode.asArray())
			{
				const auto& sheet = *sheetPtr;

				const std::wstring& name = sheet[L"name"].as<std::wstring>();
				const std::wstring& path = sheet[L"path"].as<std::wstring>();

				spk::Vector2Int size(
					sheet[L"size"][L"x"].as<long>(),
					sheet[L"size"][L"y"].as<long>());

				_spriteSheets[name] = spk::SpriteSheet(path, size);
			}
		}

		/********************  FONTS  *******************/
		{
			const auto& fontsNode = p_atlasFileConfiguration[L"fonts"];

			if (!fontsNode.isArray())
				GENERATE_ERROR("The \"fonts\" entry must be a JSON array");

			for (const spk::JSON::Object* fontPtr : fontsNode.asArray())
			{
				const auto& font = *fontPtr;

				const std::wstring& name = font[L"name"].as<std::wstring>();
				const std::wstring& path = font[L"path"].as<std::wstring>();

				_fonts[name] = spk::Font(path);
			}
		}
	}

	static spk::SafePointer<spk::Image> image(const std::wstring& p_name)
	{
		if (_images.contains(p_name) == false)
		{
			throw std::runtime_error("Image not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return &(_images[p_name]);
	}

	static spk::SafePointer<spk::SpriteSheet> spriteSheet(const std::wstring& p_name)
	{
		if (_spriteSheets.contains(p_name) == false)
		{
			throw std::runtime_error("SpriteSheet not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return &(_spriteSheets[p_name]);
	} 

	static spk::SafePointer<spk::Font> font(const std::wstring& p_name)
	{
		if (_fonts.contains(p_name) == false)
		{
			throw std::runtime_error("Font not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return &(_fonts[p_name]);
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
	TextEdit _ipTextEdit;
	PushButton _joinButton;
	PushButton _hostButton;
	PushButton _exitButton;

	void _onGeometryChange()
	{
		spk::Vector2Int availableSize = spk::Vector2Int::min(geometry().size, {600, 400});
		spk::Vector2Int position = (geometry().size - availableSize) / 2;

		const spk::Vector2Int space = {10, 10};
		spk::Vector2Int lineSize = {availableSize.x, (availableSize.y - space.y * 2)/ 3};
	
		spk::Vector2Int editSize = {lineSize.x * 0.7, lineSize.y};
		spk::Vector2Int buttonEditSize = {lineSize.x * 0.3 - space.x, lineSize.y};

		_ipTextEdit.setGeometry(position, editSize);
		_joinButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 0 + spk::Vector2Int(editSize.x + space.x, 0), buttonEditSize);
		_hostButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 1, lineSize);
		_exitButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 2, lineSize);
	}

public:
	MenuHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_ipTextEdit(p_name + L"/IpTextEdit", this),
		_joinButton(p_name + L"/JoinButton", this),
		_hostButton(p_name + L"/HostButton", this),
		_exitButton(p_name + L"/ExitButton", this)
	{
		_ipTextEdit.setPlaceholder(L"Enter IP address");
		_ipTextEdit.activate();

		_joinButton.setText(L"Join");
		_joinButton.activate();

		_hostButton.setText(L"Host game");
		_hostButton.activate();

		_exitButton.setText(L"Quit");
		_exitButton.activate();
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

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	EventDispatcher::subscribe(Event::CloseApplication, [&]() {
		app.quit(0);
	});

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::emit(Event::EnterMenuHUD);

	return (app.run());
}