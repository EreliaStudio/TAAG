#include <sparkle.hpp>

#include <random>
#include <string>
#include <climits>

#include "asset_atlas.hpp"
#include "widget_override.hpp"

struct World
{
	struct SaveFile
	{		
		static std::wstring convertWorldNameToFileName(const std::wstring& p_name)
		{
			std::wstring result = spk::StringUtils::trimWhitespace(p_name);

			std::replace(result.begin(), result.end(), L' ', L'_');

			return result;
		}

		static std::filesystem::path compose(const std::wstring& p_name)
		{
			static std::filesystem::path folderPath = "resources/saves";

			return (folderPath / convertWorldNameToFileName(p_name) / "save.json");
		}
	};
	
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

class SeedGenerator
{
public:
	static std::wstring generateRandomSeed(std::size_t len)
	{
		static std::mt19937_64 rng{ std::random_device{}() };

		static const std::vector<wchar_t> pool = []{
			std::vector<wchar_t> v;
			for (int c = 0x20;  c <= 0x7E;  ++c) v.push_back(static_cast<wchar_t>(c));
			for (int c = 0x00C0; c <= 0x00FF; ++c) v.push_back(static_cast<wchar_t>(c));
			return v;
		}();

		std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);

		std::wstring s; s.reserve(len);
		for (std::size_t i = 0; i < len; ++i)
		{
			s.push_back(pool[dist(rng)]);
		}

		return s;
	}

	static long encodeSeed(const std::wstring& s)
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
};

class InformationMessageBox : public MessageBox
{
public:
    InformationMessageBox(const std::wstring& p_name,
                          spk::SafePointer<spk::Widget> p_parent) :
        MessageBox(p_name, p_parent)
    {
        setTitle(L"Information message");
        addButton(p_name + L"/CloseButton", L"Close", [&] { deactivate(); });

        setLayer(100); 
        setMinimalWidth(0);
    }
};

class RequestMessageBox : public MessageBox
{
private:
	spk::SafePointer<spk::PushButton> _firstButton;
	spk::SafePointer<spk::PushButton> _secondButton;

public:
    RequestMessageBox(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
        MessageBox(p_name, p_parent)
    {
        setTitle(L"Request");
        setLayer(100);
        setMinimalWidth(0);

        _firstButton = addButton(p_name + L"/FirstButton",  L"FirstButton", [&]{});
        _secondButton = addButton(p_name + L"/SecondButton", L"SecondButton", [&]{});
    }

    void configure(const std::wstring& p_firstCaption, const std::function<void()>& p_firstAction,
                   const std::wstring& p_secondCaption, const std::function<void()>& p_secondAction)
    {
		_firstButton->setName(p_name + L"/" + p_firstCaption + L"Button");
        _firstButton->setText(p_firstCaption);
        _secondButton->setText(p_secondCaption);

		_secondButton->setName(p_name + L"/" + p_secondCaption + L"Button");
        _firstContract = setButtonCallback(p_firstAction);
        _secondContract = setButtonCallback(p_secondAction);
    }
};

class NewGameMenu : public spk::Screen
{
private:
    struct Content : public spk::Widget
    {
        using NameRow = FormRow<TextEdit>;
        using SeedRow = FormRow<TextEdit>;

        spk::FormLayout _layout;

        NameRow _nameRow;
        SeedRow _seedRow;
        spk::SpacerWidget _spacer;

        void _onGeometryChange() override
        {
            _layout.setGeometry({0, geometry().size});
        }

        Content(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
            spk::Widget(p_name, p_parent),
            _nameRow(p_name + L"/Name", this),
            _seedRow(p_name + L"/Seed", this),
            _spacer(p_name + L"/Spacer", this)
        {
            _layout.setElementPadding({10, 10});

            _nameRow.label.setText(L"Name:");
            _nameRow.field.setPlaceholder(L"new world");
            _nameRow.activate();

            _seedRow.label.setText(L"Seed:");
            _seedRow.field.setPlaceholder(L"000000000");
            _seedRow.activate();

            _layout.addRow(_nameRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
            _layout.addRow(_seedRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
            _layout.addRow(nullptr, &_spacer, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::Extend);
        }

        void clear()
        {
            _nameRow.field.setText(L"");
            _seedRow.field.setText(SeedGenerator::generateRandomSeed(12));
        }

        std::wstring seed() const { return _seedRow.field.text(); }
        std::wstring worldName() const { return _nameRow.field.text(); }
    };

    Context::Instanciator _contextInstanciator;

    spk::VerticalLayout _layout;
    spk::Widget::Contract _onActivationContract;

    Content _content;

    RequestMessageBox      _alreadyExistMessageBox;
    InformationMessageBox  _genericMessageBox;

    CommandPanel _commandPanel;

    void _onGeometryChange() override
    {
        spk::Vector2Int layoutSize = geometry().size / 1.2f;
        spk::Vector2Int layoutPos  = (geometry().size - layoutSize) / 2;
        _layout.setGeometry({layoutPos, layoutSize});

        auto boxSize = _alreadyExistMessageBox.minimalSize();
        auto boxPos  = (geometry().size - boxSize) / 2;
        _alreadyExistMessageBox.setGeometry(boxPos, boxSize);

        boxSize = _genericMessageBox.minimalSize();
        boxPos  = (geometry().size - boxSize) / 2;
        _genericMessageBox.setGeometry(boxPos, boxSize);
    }

    void _createWorldSave(const std::wstring& p_worldName)
    {
        const std::filesystem::path gameSaveFolder = World::SaveFile::compose(p_worldName);

        if (std::filesystem::exists(gameSaveFolder))
        {
            _alreadyExistMessageBox.setText(L"The world [" + p_worldName + L"] already exists!\nPlease enter another name or delete the old save.");
            _alreadyExistMessageBox.activate();
            return;
        }

        std::error_code ec;
        std::filesystem::create_directories(gameSaveFolder, ec);
        if (ec)
        {
            _genericMessageBox.setText(L"Unable to create save directory!\n" + spk::StringUtils::stringToWString(ec.message()));
            _genericMessageBox.activate();
            return;
        }

        Context::instance()->world.name = _content.worldName();
        Context::instance()->world.seed = SeedGenerator::encodeSeed(_content.seed());
        Context::instance()->world.save(gameSaveFolder);

        EventDispatcher::emit(Event::EnterGameMode);
    }

    void _deleteWorldSave(const std::wstring& p_worldName)
    {
        const std::filesystem::path gameSaveFolder = World::SaveFile::compose(p_worldName);

        if (!std::filesystem::exists(gameSaveFolder))
        {
            _genericMessageBox.setText(L"The world [" + p_worldName + L"] doesn't exist!\nNothing to delete.");
            _genericMessageBox.activate();
            return;
        }

        std::error_code ec;
        std::filesystem::remove_all(gameSaveFolder, ec);
        if (ec)
        {
            _genericMessageBox.setText(L"Unable to delete save directory!\n" + spk::StringUtils::stringToWString(ec.message()));
            _genericMessageBox.activate();
            return;
        }

        _genericMessageBox.setText(L"World [" + p_worldName + L"] deleted successfully.");
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
        _onActivationContract = addActivationCallback([&]
        {
            _content.clear();
            _content.activate();
            _commandPanel.activate();
            _genericMessageBox.deactivate();
            _alreadyExistMessageBox.deactivate();
        });

        _alreadyExistMessageBox.setTitle(L"Error message");
        _alreadyExistMessageBox.configure(
				L"Delete save",  [&]{ _deleteWorldSave(_content.worldName()); },
				L"Close", [&]{ _alreadyExistMessageBox.deactivate(); }
			);

        _commandPanel.addButton(L"ConfirmButton", L"Confirm", [&]{ _createWorldSave(_content.worldName()); });

        _commandPanel.addButton(L"CancelButton", L"Cancel", [&]{ EventDispatcher::emit(Event::EnterMainMenu); });

        _layout.addWidget(&_content, spk::Layout::SizePolicy::Extend);
        _layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);
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