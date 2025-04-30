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

enum class Event
{
	CloseApplication,
	EnterMenuHUD,
	EnterGameHUD
};

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

class AssetAtlas : public spk::Singleton<AssetAtlas>
{
	friend class spk::Singleton<AssetAtlas>;
private:
	/*
	{
		"Images":[
			{
				"Name":"Image",
				"Path":"assets/images/image.png"
			}
		],
		"SpriteSheets":[
			{
				"Name":"SpriteSheet",
				"Path":"assets/spritesheets/spritesheet.png",
				"Size":{
					"x":64,
					"y":64
				}
			}
		],
		"Fonts":[
			{
				"Name":"Font",
				"Path":"assets/fonts/font.ttf"
			}
		]
	}
	 */
	std::unordered_map<std::wstring, std::unique_ptr<spk::Image>> _images;
	std::unordered_map<std::wstring, std::unique_ptr<spk::SpriteSheet>> _spriteSheets;
	std::unordered_map<std::wstring, std::unique_ptr<spk::Font>> _fonts;

	AssetAtlas() = default;
	AssetAtlas(const spk::JSON::File& p_atlasFileConfiguration)
	{
		load(p_atlasFileConfiguration);
	}

public:
	void load(const spk::JSON::File& p_atlasFileConfiguration)
	{
		if (p_atlasFileConfiguration.contains(L"Images") == true)
		{
			const auto& imagesNode = p_atlasFileConfiguration[L"Images"];

			if (imagesNode.isArray() == false)
			{
				throw std::runtime_error("The \"Images\" entry must be a JSON array");
			}

			for (const spk::JSON::Object* imagePtr : imagesNode.asArray())
			{
				const auto& image = *imagePtr;

				const std::wstring& name = image[L"Name"].as<std::wstring>();
				const std::wstring& path = image[L"Path"].as<std::wstring>();

				_images[name] = std::make_unique<spk::Image>(path);
			}
		}

		if (p_atlasFileConfiguration.contains(L"SpriteSheets") == true)
		{
			const auto& sheetsNode = p_atlasFileConfiguration[L"SpriteSheets"];

			if (sheetsNode.isArray() == false)
			{
				throw std::runtime_error("The \"SpriteSheets\" entry must be a JSON array");
			}

			for (const spk::JSON::Object* sheetPtr : sheetsNode.asArray())
			{
				const auto& sheet = *sheetPtr;

				const std::wstring& name = sheet[L"Name"].as<std::wstring>();
				const std::wstring& path = sheet[L"Path"].as<std::wstring>();

				spk::Vector2Int size(
					sheet[L"Size"][L"x"].as<long>(),
					sheet[L"Size"][L"y"].as<long>());

				_spriteSheets[name] = std::make_unique<spk::SpriteSheet>(path, size);
			}
		}

		if (p_atlasFileConfiguration.contains(L"Fonts") == true)
		{
			const auto& fontsNode = p_atlasFileConfiguration[L"Fonts"];

			if (fontsNode.isArray() == false)
			{
				throw std::runtime_error("The \"Fonts\" entry must be a JSON array");
			}

			for (const spk::JSON::Object* fontPtr : fontsNode.asArray())
			{
				const auto& font = *fontPtr;

				const std::wstring& name = font[L"Name"].as<std::wstring>();
				const std::wstring& path = font[L"Path"].as<std::wstring>();

				_fonts[name] = std::make_unique<spk::Font>(path);
			}
		}
	}

	spk::SafePointer<spk::Image> image(const std::wstring& p_name)
	{
		if (_images.contains(p_name) == false)
		{
			throw std::runtime_error("Image not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return (_images[p_name].get());
	}

	spk::SafePointer<spk::SpriteSheet> spriteSheet(const std::wstring& p_name)
	{
		if (_spriteSheets.contains(p_name) == false)
		{
			throw std::runtime_error("SpriteSheet not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return (_spriteSheets[p_name].get());
	}

	spk::SafePointer<spk::Font> font(const std::wstring& p_name)
	{
		if (_fonts.contains(p_name) == false)
		{
			throw std::runtime_error("Font not found: " + spk::StringUtils::wstringToString(p_name));
		}
		return (_fonts[p_name].get());
	}
};

class PushButton : public spk::PushButton
{
private:
	AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
	PushButton(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::PushButton(p_name, p_parent)
	{
		setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"), PushButton::State::Released);
		setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice_Dark"), PushButton::State::Pressed);
		setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice_Light"), PushButton::State::Hovered);

		setFont(AssetAtlas::instance()->font(L"defaultFont"));
		setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
		setFontSize({16, 3});
	}
};

class TextLabel : public spk::TextLabel
{
private:
	AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
	TextLabel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextLabel(p_name, p_parent)
	{
		setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));

		setFont(AssetAtlas::instance()->font(L"defaultFont"));

		setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
		setFontSize({16, 3});
	}
};

class TextEdit : public spk::TextEdit
{
private:
	AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
	TextEdit(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextEdit(p_name, p_parent)
	{
		setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));

		setFont(AssetAtlas::instance()->font(L"defaultFont"));

		setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
		setFontSize({16, 3});
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
	TextEdit _addressTextEdit;
	PushButton _joinButton;
	PushButton::Contract _joinButtonContract;

	PushButton _hostButton;
	PushButton::Contract _hostButtonContract;

	PushButton _exitButton;
	PushButton::Contract _exitButtonContract;

	void _onGeometryChange()
	{
		spk::Vector2Int availableSize = spk::Vector2Int::min(geometry().size, {600, 400});
		spk::Vector2Int position = (geometry().size - availableSize) / 2;

		const spk::Vector2Int space = {10, 10};
		spk::Vector2Int lineSize = {availableSize.x, (availableSize.y - space.y * 2)/ 3};
	
		spk::Vector2Int editSize = {lineSize.x * 0.7, lineSize.y};
		spk::Vector2Int buttonEditSize = {lineSize.x * 0.3 - space.x, lineSize.y};

		_addressTextEdit.setGeometry(position, editSize);
		_joinButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 0 + spk::Vector2Int(editSize.x + space.x, 0), buttonEditSize);
		_hostButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 1, lineSize);
		_exitButton.setGeometry(position + spk::Vector2Int(0, lineSize.y + space.y) * 2, lineSize);
	}

	void _onJoinRequest()
	{
		Client::instanciate();

		std::wstring address = _addressTextEdit.text();
		Client::instance()->setup(spk::StringUtils::wstringToString(address));
	}

	void _onHostRequest()
	{

	}

public:
	MenuHUD(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_addressTextEdit(p_name + L"/IpTextEdit", this),
		_joinButton(p_name + L"/JoinButton", this),
		_hostButton(p_name + L"/HostButton", this),
		_exitButton(p_name + L"/ExitButton", this)
	{
		_addressTextEdit.setPlaceholder(L"Enter IP address");
		_addressTextEdit.activate();

		_joinButton.setText(L"Join");
		_joinButtonContract = _joinButton.subscribe([&]() {
			_onJoinRequest();
		});
		_joinButton.activate();

		_hostButton.setText(L"Host game");
		_hostButtonContract = _hostButton.subscribe([&]() {
			_onHostRequest();
		});
		_hostButton.activate();

		_exitButton.setText(L"Quit");
		_exitButtonContract = _exitButton.subscribe([&]() {
			EventDispatcher::instance()->emit(Event::CloseApplication);
		});
		_exitButton.activate();
	}
};

class MainWidget : public spk::Widget
{
private:
	spk::JSON::File _configurationFile;
	AssetAtlas::Instanciator _assetAtlasInstanciator;
	EventDispatcher::Instanciator _eventDispatcherInstanciator;

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
		_configurationFile(L"resources/configuration.json"),
		_assetAtlasInstanciator(spk::JSON::File(_configurationFile[L"AssetsConfig"].as<std::wstring>())),
		_gameHUD(p_name + L"/GameHUD", this),
		_menuHUD(p_name + L"/MenuHUD", this)
	{

		_gameHUDContract = EventDispatcher::instance()->subscribe(Event::EnterGameHUD, [&]() {
			_gameHUD.activate();
		});

		_menuHUDContract = EventDispatcher::instance()->subscribe(Event::EnterMenuHUD, [&]() {
			_menuHUD.activate();
		});
	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	EventDispatcher::Instanciator eventDispatcherInstanciator;

	EventDispatcher::Contract closeApplicationContract = EventDispatcher::instance()->subscribe(Event::CloseApplication, [&]() {
		app.quit(0);
	});

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::instance()->emit(Event::EnterMenuHUD);

	return (app.run());
}