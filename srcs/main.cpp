#include <sparkle.hpp>

#include <random>
#include <string>
#include <climits>

#include "asset_atlas.hpp"
#include "widget_override.hpp"

struct World
{
	std::wstring name;
	long seed;

	void load(const std::filesystem::path& p_folderPath)
	{
		spk::JSON::File inputFile = spk::JSON::File(p_folderPath / "save.json");

		name = inputFile[L"World name"].as<std::wstring>();
		seed = inputFile[L"Seed"].as<long>();
	}

	void save(const std::filesystem::path& p_folderPath)
	{
		spk::JSON::File outputFile;

		outputFile.root().addAttribute(L"World name") = name;
		outputFile.root().addAttribute(L"Seed") = seed;

		outputFile.save(p_folderPath / "save.json");
	}
};

struct Context : public spk::Singleton<Context>
{
	World world;
};

enum class Event
{
	ExitGame,
	EnterMainMenu,
	EnterNewGameMenu,
	EnterLoadGameMenu,
	EnterOptionMenu,
	EnterGameMode
};
using EventDispatcher = spk::EventDispatcher<Event>;

class MainMenu : public spk::Screen
{
private:
	spk::VerticalLayout _layout;

	PushButton _newGameButton;
	PushButton _loadGameButton;
	PushButton _optionsButton;
	PushButton _exitButton;

	void _onGeometryChange()
	{
		spk::Vector2Int layoutSize = geometry().size / 2;
		spk::Vector2Int layoutPos = (geometry().size - layoutSize) / 2;

		_layout.setGeometry({layoutPos, layoutSize});
	}

public:
	MainMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_newGameButton(L"NewGameButton", this),
		_loadGameButton(L"LoadGameButton", this),
		_optionsButton(L"OptionsButton", this),
		_exitButton(L"ExitButton", this)
	{
		_layout.addWidget(&_newGameButton);
		_layout.addWidget(&_loadGameButton);
		_layout.addWidget(&_optionsButton);
		_layout.addWidget(&_exitButton);
		_layout.setElementPadding({10, 10});

		_newGameButton.setText(L"New Game");
		_newGameButton.setOnClick([&]()
		{
			EventDispatcher::emit(Event::EnterNewGameMenu);
		});

		_loadGameButton.setText(L"Load Game");
		_loadGameButton.setOnClick([&]()
		{
			EventDispatcher::emit(Event::EnterLoadGameMenu);
		});

		_optionsButton.setText(L"Options");
		_optionsButton.setOnClick([&]()
		{
			EventDispatcher::emit(Event::EnterOptionMenu);
		});

		_exitButton.setText(L"Exit");
		_exitButton.setOnClick([&](){EventDispatcher::emit(Event::ExitGame);});

		_newGameButton.activate();
		_loadGameButton.activate();
		_optionsButton.activate();
		_exitButton.activate();
	}
};

class NewGameMenu : public spk::Screen
{
private:
	struct Content : public spk::Widget
	{
	private:
		spk::FormLayout _layout;

		TextLabel _nameLabel;
		TextEdit _nameEdit;

		TextLabel _seedLabel;
		TextEdit _seedEdit;
		spk::SpacerWidget _spacer;

		void _onGeometryChange()
		{
			_layout.setGeometry({0, geometry().size});
		}

		std::wstring _generateRandomSeed(std::size_t len)
		{
			static std::mt19937_64 rng{ std::random_device{}() };

			static const std::vector<wchar_t> pool = []{
				std::vector<wchar_t> v;
				for (int c = 0x20; c <= 0x7E;  ++c) v.push_back(static_cast<wchar_t>(c));
				for (int c = 0x00C0; c <= 0x00FF; ++c) v.push_back(static_cast<wchar_t>(c));
				return v;
			}();

			std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);

			std::wstring s;
			s.reserve(len);
			for (std::size_t i = 0; i < len; ++i)
			{
				s.push_back(pool[dist(rng)]);
			}

			return s;
		}

	public:
		Content(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
			spk::Widget(p_name, p_parent),
			_nameLabel(p_name + L"/NameLabel", this),
			_nameEdit(p_name + L"/NameEdit", this),
			_seedLabel(p_name + L"/SeedLabel", this),
			_seedEdit(p_name + L"/SeedEdit", this),
			_spacer(p_name + L"/Spacer", this)
		{
			_layout.setElementPadding({10, 10});

			_nameLabel.setText(L"Name:");
			_nameEdit.setPlaceholder(L"new world");

			_seedLabel.setText(L"Seed:");
			_seedEdit.setPlaceholder(L"000000000");

			_layout.addRow(&_nameLabel,&_nameEdit, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
			_layout.addRow(&_seedLabel,&_seedEdit, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
			_layout.addRow(nullptr,&_spacer, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::Extend);

			_nameLabel.activate();
			_nameEdit.activate();
			_seedLabel.activate();
			_seedEdit.activate();
			_spacer.activate();
		}

		void clear()
		{
			_nameEdit.setText(L"");

			_seedEdit.setText(_generateRandomSeed(12));
		}

		std::wstring seed() const
		{
			return (_seedEdit.text());
		}

		std::wstring worldName() const
		{
			return (_nameEdit.text());
		}
	};

	Context::Instanciator _contextInstanciator;
	spk::VerticalLayout _layout;

	spk::Widget::Contract _onActivationContract;

	Content _content;
	MessagePopup _alreadyExistMessageBox;
	spk::SafePointer<spk::PushButton> _deletionPushButton;
	spk::PushButton::Contract _onDeletionContract;
	MessagePopup _genericMessageBox;
	CommandPanel _commandPanel;
	CommandPanel::Contract _confirmContract;
	CommandPanel::Contract _cancelContract;

	void _onGeometryChange()
	{
		spk::Vector2Int layoutSize = geometry().size / 1.2f;
		spk::Vector2Int layoutPos = (geometry().size - layoutSize) / 2;

		_layout.setGeometry({layoutPos, layoutSize});

		if (_alreadyExistMessageBox.isActive() == true)
		{
			_alreadyExistMessageBox.setMinimumContentSize(_alreadyExistMessageBox.content()->minimalSize());
			spk::Vector2Int messageBoxSize = _alreadyExistMessageBox.minimalSize();
			spk::Vector2Int messageBoxAnchor = (geometry().size - messageBoxSize) / 2;
			_alreadyExistMessageBox.setGeometry(messageBoxAnchor, messageBoxSize);
		}

		if (_genericMessageBox.isActive() == true)
		{
			_genericMessageBox.setMinimumContentSize(_genericMessageBox.content()->minimalSize());
			spk::Vector2Int messageBoxSize = _genericMessageBox.minimalSize();
			spk::Vector2Int messageBoxAnchor = (geometry().size - messageBoxSize) / 2;
			_genericMessageBox.setGeometry(messageBoxAnchor, messageBoxSize);
		}
	}

	long _encodeSeed(const std::wstring& s)
	{
		constexpr uint64_t fnvOffset = 14695981039346656037ULL; // FNV offset
		constexpr uint64_t fnvPrime = 1099511628211ULL; // FNV prime
		uint64_t result = fnvOffset;
		for (wchar_t c : s) {
			result ^= static_cast<uint64_t>(c);
			result *= fnvPrime;
		}
		return static_cast<long>(result & LONG_MAX);
	}

	std::wstring _convertWorldNameToFileName(const std::wstring& p_worldName) const
	{
		std::wstring result = p_worldName;

		auto trim = [](std::wstring& s)
		{
			const std::wstring whitespace = L" \t";
			const size_t first = s.find_first_not_of(whitespace);
			const size_t last  = s.find_last_not_of(whitespace);

			if (first == std::wstring::npos)
			{
				s.clear();
				return;
			}
			s = s.substr(first, last - first + 1);
		};
		trim(result);

		std::replace(result.begin(), result.end(), L' ', L'_');

		return result;
	}

	std::filesystem::path _composeSaveFolderPath(const std::wstring& fileName)
	{
		return (std::filesystem::path("resources/saves") / _convertWorldNameToFileName(fileName));
	}

	void _createWorldSave(const std::wstring& p_worldName)
	{
		const std::wstring fileName = _convertWorldNameToFileName(p_worldName);
		const std::filesystem::path gameSaveFolder = _composeSaveFolderPath(fileName);

		if (std::filesystem::exists(gameSaveFolder))
		{
			_alreadyExistMessageBox.setLineText(0, L"The world [" + p_worldName + L"] already exists!");
			_alreadyExistMessageBox.setLineText(1, L"Please enter another name or delete the old save.");
			_alreadyExistMessageBox.activate();
			requireGeometryUpdate();
			return;
		}

		std::error_code ec;
		std::filesystem::create_directories(gameSaveFolder, ec);
		if (ec)
		{
			_genericMessageBox.setLineText(0, L"Unable to create save directory!");
			_genericMessageBox.setLineText(1, spk::StringUtils::stringToWString(ec.message()));
			_genericMessageBox.activate();
			requireGeometryUpdate();
			return;
		}

		Context::instance()->world.name = _content.worldName();
		Context::instance()->world.seed = _encodeSeed(_content.seed());

		Context::instance()->world.save(gameSaveFolder);

		EventDispatcher::emit(Event::EnterGameMode);
	}

	void _deleteWorldSave(const std::wstring& p_worldName)
	{
		const std::wstring fileName = _convertWorldNameToFileName(p_worldName);
		const std::filesystem::path gameSaveFolder = _composeSaveFolderPath(fileName);

		if (!std::filesystem::exists(gameSaveFolder))
		{
			_genericMessageBox.setLineText(0, L"The world [" + p_worldName + L"] doesn’t exist!");
			_genericMessageBox.setLineText(1, L"Nothing to delete.");
			_genericMessageBox.activate();
			requireGeometryUpdate();
			return;
		}

		std::error_code ec;
		std::filesystem::remove_all(gameSaveFolder, ec);

		if (ec)
		{
			_genericMessageBox.setLineText(0, L"Unable to delete save directory!");
			_genericMessageBox.setLineText(1, spk::StringUtils::stringToWString(ec.message()));
			_genericMessageBox.activate();
			requireGeometryUpdate();
			return;
		}

		_genericMessageBox.setLineText(0, L"World [" + p_worldName + L"] deleted successfully.");
		_genericMessageBox.setLineText(1, L"");
		_genericMessageBox.activate();
		_alreadyExistMessageBox.deactivate();

		requireGeometryUpdate();

	}

public:
	NewGameMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_content(p_name + L"/Content", this),
		_commandPanel(p_name + L"/CommandPanel", this),
		_genericMessageBox(p_name + L"/GenericMessageBox", this),
		_alreadyExistMessageBox(p_name + L"/AlreadyExistMessageBox", this)
	{
		_deletionPushButton = _alreadyExistMessageBox.content()->addButton(L"DeleteButton", L"Delete save");
		_onDeletionContract = _deletionPushButton->subscribe([&](){

		});

		_onActivationContract = addActivationCallback([&](){
			_content.clear();
			_genericMessageBox.deactivate();
			_genericMessageBox.deactivate();
		});

		_layout.addWidget(&_content, spk::Layout::SizePolicy::Extend);
		_layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);

		spk::SafePointer<spk::PushButton> confirmButton = _commandPanel.addButton(L"ConfirmButton", L"Confirm");
		WidgetAddons::ApplyFormat(confirmButton);
		
		spk::SafePointer<spk::PushButton> cancelButton = _commandPanel.addButton(L"CancelButton", L"Cancel");
		WidgetAddons::ApplyFormat(cancelButton);

		_genericMessageBox.setLayer(100);
		_alreadyExistMessageBox.setLayer(100);

		_confirmContract = _commandPanel.subscribe(L"ConfirmButton", [&]()
		{
			_createWorldSave(_content.worldName());
		});
		_cancelContract = _commandPanel.subscribe(L"CancelButton", [&](){EventDispatcher::emit(Event::EnterMainMenu);});

		_content.activate();
		_commandPanel.activate();
	}
};

class LoadGameMenu : public spk::Screen
{
private:

public:
	LoadGameMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{
		
	}
};

class OptionsMenu : public spk::Screen
{
private:

public:
	OptionsMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent)
	{
		
	}
};

class MainWidget : public spk::Widget
{
private:
	MainMenu _mainMenu;
	EventDispatcher::Contract _mainMenuActivationContract;

	NewGameMenu _newGameMenu;
	EventDispatcher::Contract _newGameMenuActivationContract;

	LoadGameMenu _loadGameMenu;
	EventDispatcher::Contract _loadGameMenuActivationContract;

	OptionsMenu _optionsMenu;
	EventDispatcher::Contract _optionsMenuActivationContract;

	void _onGeometryChange()
	{
		_mainMenu.setGeometry(geometry());
		_newGameMenu.setGeometry(geometry());
		_loadGameMenu.setGeometry(geometry());
		_optionsMenu.setGeometry(geometry());
	}

public:
	MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent),
		_mainMenu(p_name + L"/MainMenu", this),
		_newGameMenu(p_name + L"/NewGameMenu", this),
		_loadGameMenu(p_name + L"/LoadGameMenu", this),
		_optionsMenu(p_name + L"/OptionsMenu", this)
	{
		_mainMenuActivationContract = EventDispatcher::subscribe(Event::EnterMainMenu, [&](){_mainMenu.activate();});
		_newGameMenuActivationContract = EventDispatcher::subscribe(Event::EnterNewGameMenu, [&](){_newGameMenu.activate();});
		_loadGameMenuActivationContract = EventDispatcher::subscribe(Event::EnterLoadGameMenu, [&](){_loadGameMenu.activate();});
		_optionsMenuActivationContract = EventDispatcher::subscribe(Event::EnterOptionMenu, [&](){_optionsMenu.activate();});
	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	EventDispatcher::instanciate();

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	EventDispatcher::Contract exitContract = EventDispatcher::subscribe(Event::ExitGame, [&app]()
	{
		app.quit(0);
	});

	EventDispatcher::emit(Event::EnterMainMenu);

	return (app.run());
}