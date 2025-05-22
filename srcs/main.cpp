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

			return (folderPath / convertWorldNameToFileName(p_name));
		}
	};
	
	std::wstring name;
	long seed;
	size_t icon;

	void load(const std::filesystem::path& p_folderPath)
	{
		spk::JSON::File inputFile = spk::JSON::File(p_folderPath / "save.json");

		name = inputFile[L"World name"].as<std::wstring>();
		seed = inputFile[L"Seed"].as<long>();
		icon = inputFile[L"Icon"].as<long>();
	}

	void save(const std::filesystem::path& p_folderPath)
	{
		spk::JSON::File outputFile;

		outputFile.root().addAttribute(L"World name") = name;
		outputFile.root().addAttribute(L"Seed") = seed;
		outputFile.root().addAttribute(L"Icon") = static_cast<long>(icon);

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

class IconRenderer : public spk::Widget
{
private:
	spk::NineSliceRenderer _frameRenderer;
	spk::Vector2Int _cornerSize;

	spk::TextureRenderer _iconRenderer;
	size_t _icon;

	void _onGeometryChange() override
	{
		_frameRenderer.clear();
		_frameRenderer.prepare(geometry(), layer(), _cornerSize);
		_frameRenderer.validate();
		
		_iconRenderer.clear();
		auto spriteSheet = _iconRenderer.texture().upCast<const spk::SpriteSheet>();
		spk::Vector2UInt iconRendererSize = geometry().height - _cornerSize.y * 2;
		spk::Vector2UInt iconRendererAnchor = geometry().anchor + (geometry().size - iconRendererSize) / 2;
		_iconRenderer.prepare({iconRendererAnchor, iconRendererSize}, spriteSheet->sprite(_icon), layer() + 0.0001f);
		_iconRenderer.validate();
	}

	void _onPaintEvent(spk::PaintEvent &p_event) override
	{
		_frameRenderer.render();
		_iconRenderer.render();
	}

public:
	IconRenderer(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent)
	{
		setCornerSize({10, 10});
		setIcon(0);
	}

	void setNineSlice(const spk::SafePointer<const spk::SpriteSheet> &p_spriteSheet)
	{
		_frameRenderer.setSpriteSheet(p_spriteSheet);
	}

	spk::SafePointer<const spk::SpriteSheet> nineslice() const
	{
		return _frameRenderer.spriteSheet();
	}

	void setCornerSize(const spk::Vector2Int& p_size)
	{
		_cornerSize = p_size;
		requireGeometryUpdate();
	}

	const spk::Vector2Int &cornerSize() const
	{
		return _cornerSize;
	}

	void setIconset(spk::SafePointer<spk::SpriteSheet> p_iconset)
	{
		_iconRenderer.setTexture(p_iconset);
	}

	spk::SafePointer<const spk::SpriteSheet> iconset() const
	{
		return _iconRenderer.texture().upCast<const spk::SpriteSheet>();
	}

	void setIcon(size_t p_icon)
	{
		_icon = p_icon;
		requireGeometryUpdate();
	}

	const size_t &icon() const
	{
		return _icon;
	}
};

class IconSelector : public spk::Widget
{
private:
	spk::HorizontalLayout _layout;

	PushButton _incrementButton;
	PushButton::Contract _incrementButtonContract;
	IconRenderer _iconRenderer;
	PushButton _decrementButton;
	PushButton::Contract _decrementButtonContract;

	void _onGeometryChange()
	{
		spk::Vector2UInt buttonSize = geometry().height;
		_incrementButton.setMinimalSize(buttonSize);
		_decrementButton.setMinimalSize(buttonSize);
		_layout.setGeometry({0, geometry().size});
	}

public:
	IconSelector(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent),
		_incrementButton(p_name + L"/IncrementButton", this),
		_iconRenderer(p_name + L"/IconRenderer", this),
		_decrementButton(p_name + L"/DecrementButton", this)
	{
		_decrementButton.setIconset(spk::Widget::defaultIconset());
		_decrementButton.setIcon(spk::Widget::defaultIconset()->sprite(6));
		_decrementButtonContract = _decrementButton.subscribe([&](){
			size_t icon = _iconRenderer.icon();
			if (icon == 0)
				icon = _iconRenderer.iconset()->sprites().size() - 1;
			else
				icon--;
			_iconRenderer.setIcon(icon);
		});
		_decrementButton.activate();
		
		_iconRenderer.setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
		_iconRenderer.setIconset(AssetAtlas::instance()->spriteSheet(L"saveIcons"));
		_iconRenderer.setIcon(0);
		_iconRenderer.activate();

		_incrementButton.setIconset(spk::Widget::defaultIconset());
		_incrementButton.setIcon(spk::Widget::defaultIconset()->sprite(7));
		_incrementButtonContract = _incrementButton.subscribe([&](){
			size_t icon = _iconRenderer.icon();
			if (icon == _iconRenderer.iconset()->sprites().size() - 1)
				icon = 0;
			else
				icon++;
			_iconRenderer.setIcon(icon);
		});
		_incrementButton.activate();

		_layout.setElementPadding({10, 10});
		_layout.addWidget(&_decrementButton, spk::Layout::SizePolicy::Minimum);
		_layout.addWidget(&_iconRenderer, spk::Layout::SizePolicy::HorizontalExtend);
		_layout.addWidget(&_incrementButton, spk::Layout::SizePolicy::Minimum);
	}

	void setIconset(spk::SafePointer<spk::SpriteSheet> p_iconset)
	{
		_iconRenderer.setIconset(p_iconset);
	}
	
	spk::SafePointer<const spk::SpriteSheet> iconset() const
	{
		return _iconRenderer.iconset();
	}

	void setIcon(size_t p_icon)
	{
		_iconRenderer.setIcon(p_icon);
	}

	const size_t &icon() const
	{
		return _iconRenderer.icon();
	}

	spk::Vector2UInt minimalSize() const override
	{
		spk::Vector2UInt decrementMinimalSize = _decrementButton.minimalSize();
		spk::Vector2UInt incrementMinimalSize = _incrementButton.minimalSize();
		spk::Vector2UInt iconMinimalSize = _iconRenderer.minimalSize();

		return spk::Vector2UInt(
			decrementMinimalSize.x + _layout.elementPadding().x + incrementMinimalSize.x + _layout.elementPadding().x + iconMinimalSize.x,
			std::max({decrementMinimalSize.y, incrementMinimalSize.y, iconMinimalSize.y})
		);
	}
};

class NewGameMenu : public spk::Screen
{
private:
    struct Content : public spk::Widget
    {
        using NameRow = FormRow<TextEdit>;
        using SeedRow = FormRow<TextEdit>;
        using IconRow = FormRow<IconSelector>;

        spk::FormLayout _layout;

        NameRow _nameRow;
        SeedRow _seedRow;
		IconRow _iconRow;

        void _onGeometryChange() override
        {
            _layout.setGeometry({0, geometry().size});
        }

        Content(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
            spk::Widget(p_name, p_parent),
            _nameRow(p_name + L"/Name", this),
            _seedRow(p_name + L"/Seed", this),
			_iconRow(p_name + L"/Icon", this)
        {
            _layout.setElementPadding({10, 10});

            _nameRow.label.setText(L"Name:");
            _nameRow.field.setPlaceholder(L"new world");
            _nameRow.activate();

            _seedRow.label.setText(L"Seed:");
            _seedRow.field.setPlaceholder(L"000000000");
            _seedRow.activate();

            _iconRow.label.setText(L"Icon:");
            _iconRow.field.setIcon(0);
            _iconRow.activate();

            _layout.addRow(_nameRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
            _layout.addRow(_seedRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
            _layout.addRow(_iconRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
        }

        void clear()
        {
            _nameRow.field.setText(L"");
            _seedRow.field.setText(SeedGenerator::generateRandomSeed(12));
			_iconRow.field.setIcon(0);
        }

        std::wstring seed() const { return _seedRow.field.text(); }
        std::wstring worldName() const { return _nameRow.field.text(); }
        size_t iconID() const { return _iconRow.field.icon(); }

		spk::Vector2UInt minimalSize() const override
		{
			return (_layout.minimalSize());
		}
    };

    Context::Instanciator _contextInstanciator;

    spk::VerticalLayout _layout;
    spk::Widget::Contract _onActivationContract;

    Content _content;

    spk::SpacerWidget _spacer;

    RequestMessageBox _alreadyExistMessageBox;
    InformationMessageBox  _genericMessageBox;

    CommandPanel _commandPanel;

    void _onGeometryChange() override
    {
		WidgetAddons::centerInParent(&_layout, geometry().size / 1.2f, geometry());
		WidgetAddons::centerInParent(&_alreadyExistMessageBox, _alreadyExistMessageBox.minimalSize(), geometry());
		WidgetAddons::centerInParent(&_genericMessageBox, _genericMessageBox.minimalSize(), geometry());
    }

    void _createWorldSave(const std::wstring& p_worldName)
    {
        const std::filesystem::path gameSaveFolder = World::SaveFile::compose(p_worldName);

        if (std::filesystem::exists(gameSaveFolder))
        {
            _alreadyExistMessageBox.show(L"The world [" + p_worldName + L"] already exists!\nPlease enter another name or delete the old save.");
            return;
        }

        std::error_code ec;
        std::filesystem::create_directories(gameSaveFolder, ec);
        if (ec)
        {
            _genericMessageBox.show(L"Unable to create save directory!\n" + spk::StringUtils::stringToWString(ec.message()));
            return;
        }

        Context::instance()->world.name = _content.worldName();
        Context::instance()->world.seed = SeedGenerator::encodeSeed(_content.seed());
        Context::instance()->world.icon = _content.iconID();
		
        Context::instance()->world.save(gameSaveFolder);

        EventDispatcher::emit(Event::EnterGameMode);
    }

    void _deleteWorldSave(const std::wstring& p_worldName)
    {
        const std::filesystem::path gameSaveFolder = World::SaveFile::compose(p_worldName);

        if (!std::filesystem::exists(gameSaveFolder))
        {
            _genericMessageBox.show(L"The world [" + p_worldName + L"] doesn't exist!\nNothing to delete.");
            return;
        }

        std::error_code ec;
        std::filesystem::remove_all(gameSaveFolder, ec);
        if (ec)
        {
            _genericMessageBox.show(L"Unable to delete save directory!\n" + spk::StringUtils::stringToWString(ec.message()));
            return;
        }

        _genericMessageBox.show(L"World [" + p_worldName + L"] deleted successfully.");
        _alreadyExistMessageBox.deactivate();
        requireGeometryUpdate();
    }

public:
    NewGameMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
        spk::Screen(p_name, p_parent),
        _content(p_name + L"/Content", this),
        _commandPanel(p_name + L"/CommandPanel", this, {
			{L"Confirm", [&]{ _createWorldSave(_content.worldName());}},
			{L"Cancel",  [&]{ EventDispatcher::emit(Event::EnterMainMenu); }}
		}),
        _genericMessageBox(p_name + L"/GenericMessageBox", this),
        _alreadyExistMessageBox(p_name + L"/AlreadyExistMessageBox", this),
    	_spacer(p_name + L"/Spacer", this)
    {
		_layout.setElementPadding({10, 10});

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
		_alreadyExistMessageBox.setLayer(100);
		_alreadyExistMessageBox.setMinimalWidth(0);

		_genericMessageBox.setLayer(100);
		_genericMessageBox.setMinimalWidth(0);

        _layout.addWidget(&_content, spk::Layout::SizePolicy::Minimum);
        _layout.addWidget(&_spacer, spk::Layout::SizePolicy::Extend);
        _layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);
    }
};

class LoadGameMenu : public spk::Screen
{
private:
	struct Content : public spk::ScrollableWidget
	{
	private:

		void _onGeometryChange()
		{

		}

	public:
		Content(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
			spk::ScrollableWidget(p_name, p_parent)
		{

		}

		void updateSaveFileList()
		{
			struct SaveEntry
			{
				std::filesystem::path folder;
				std::filesystem::file_time_type time;
			};

			std::vector<SaveEntry> saves;
			for (const auto& folder : spk::FileUtils::listFolders("resources/saves"))
			{
				const auto savePath = folder / "save.json";
				if (std::filesystem::exists(savePath))
				{
					saves.push_back({ folder, std::filesystem::last_write_time(savePath) });
				}
			}

			std::sort(saves.begin(), saves.end(),
					[](const SaveEntry& a, const SaveEntry& b)
					{
						return a.time < b.time;
					});

			spk::cout << "Save :" << std::endl;
			for (const auto& entry : saves)
			{
				auto sysTime  = std::chrono::file_clock::to_sys(entry.time);
				std::time_t   cTime    = std::chrono::system_clock::to_time_t(sysTime);

				std::wostringstream timeStr;
				timeStr << std::put_time(std::localtime(&cTime), L"%Y-%m-%d %H:%M:%S");

				// ---------- read world name & print ----------
				spk::JSON::File saveFile(entry.folder / "save.json");
				spk::cout << L"    "
						<< saveFile[L"World name"].as<std::wstring>()
						<< L"  (" << timeStr.str() << L')'
						<< std::endl;
			}
		}

		spk::Vector2UInt requiredSize() const override
		{
			return {100u, 100u};
		}
	};
	
	spk::ContractProvider::Contract _onActivationContract;

	spk::VerticalLayout _layout;

	spk::ScrollArea<Content> _content;
    CommandPanel _commandPanel;

	void _onGeometryChange()
	{
		WidgetAddons::centerInParent(&_layout, geometry().size / 1.2f, geometry());
	}

public:
	LoadGameMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_content(p_name + L"/Content", this),
        _commandPanel(p_name + L"/CommandPanel", this, {
			{L"Cancel",  [&]{ EventDispatcher::emit(Event::EnterMainMenu); }}
		})
	{
		_onActivationContract = addActivationCallback([&]
		{
			_content.content()->updateSaveFileList();
			requireGeometryUpdate();
		});
		_layout.addWidget(&_content, spk::Layout::SizePolicy::Extend);
		_layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);

		_content.activate();
		_commandPanel.activate();
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
		_mainMenuActivationContract = EventDispatcher::subscribe(Event::EnterMainMenu, [&](){_mainMenu.activate(); requestPaint();});
		_newGameMenuActivationContract = EventDispatcher::subscribe(Event::EnterNewGameMenu, [&](){_newGameMenu.activate();requestPaint();});
		_loadGameMenuActivationContract = EventDispatcher::subscribe(Event::EnterLoadGameMenu, [&](){_loadGameMenu.activate();requestPaint();});
		_optionsMenuActivationContract = EventDispatcher::subscribe(Event::EnterOptionMenu, [&](){_optionsMenu.activate();requestPaint();});
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