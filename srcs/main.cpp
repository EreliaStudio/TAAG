#include <sparkle.hpp>

#include "asset_atlas.hpp"
#include "widget_override.hpp"

enum class Event
{
	ExitGame,
	EnterMainMenu,
	EnterNewGameMenu,
	EnterLoadGameMenu,
	EnterOptionMenu
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
		spk::SpacerWidget _spacer;

		void _onGeometryChange()
		{
			_layout.setGeometry({0, geometry().size});
		}

	public:
		Content(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
			spk::Widget(p_name, p_parent),
			_nameLabel(p_name + L"/NameLabel", this),
			_nameEdit(p_name + L"/NameEdit", this),
			_spacer(p_name + L"/Spacer", this)
		{
			_layout.setElementPadding({10, 10});

			_nameLabel.setText(L"Name:");

			_nameEdit.setPlaceholder(L"new world");

			_layout.addRow(&_nameLabel,&_nameEdit, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
			_layout.addRow(nullptr,&_spacer, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::Extend);

			_nameLabel.activate();
			_nameEdit.activate();
			_spacer.activate();
		}

		void clear()
		{
			_nameEdit.setText(L"");
		}

		std::wstring worldName() const
		{
			return (_nameEdit.text());
		}

		std::wstring subfolderName() const
		{
			std::wstring result = worldName();

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
	};

	spk::VerticalLayout _layout;

	spk::Widget::Contract _onActivationContract;

	Content _content;
	MessagePopup _messageBox;
	CommandPanel _commandPanel;
	CommandPanel::Contract _confirmContract;
	CommandPanel::Contract _cancelContract;

	void _onGeometryChange()
	{
		spk::Vector2Int layoutSize = geometry().size / 1.2f;
		spk::Vector2Int layoutPos = (geometry().size - layoutSize) / 2;

		_layout.setGeometry({layoutPos, layoutSize});

		_messageBox.setMinimumContentSize(_messageBox.content()->minimalSize());
		spk::Vector2Int messageBoxSize = _messageBox.minimalSize();
		spk::Vector2Int messageBoxAnchor = (geometry().size - messageBoxSize) / 2;
		_messageBox.setGeometry(messageBoxAnchor, messageBoxSize);
	}

	void _tryLaunchGame()
	{
		const std::filesyetem::path gameSaveFolder = path("resources/saves") / _content.subfolderName();

		// ── 1) If the folder already exists, show an error in the message box ─────
		if (std::filesyetem::exists(gameSaveFolder))
		{
			_messageBox.setLineText(0, L"The chosen world already exists!");
			_messageBox.setLineText(1, L"Please enter another name or delete the old save.");
			_messageBox.activate();
			return;
		}

		// ── 2) Create the directory hierarchy (resources/saves/<folder>) ──────────
		std::error_code ec;
		std::filesyetem::create_directories(gameSaveFolder, ec);
		if (ec)
		{
			_messageBox.setLineText(0, L"Unable to create save directory!");
			_messageBox.setLineText(1, spk::StringUtils::stringToWstring(ec.message()));
			_messageBox.activate();
			return;
		}

		// ── 3) Seed a fresh save.json with the world name ─────────────────────────
		spk::JSON::File saveFile;

		// File::root() was declared const in the header you pasted, so either:
		//  a) add a non-const overload, or
		//  b) build a temporary Object and assign it.
		// One clean way is to expose a non-const root() in File.  Assuming that:
		//
		saveFile.root().addAttribute(L"World name") = _content.worldName();

		saveFile.save(gameSaveFolder / "save.json");
	}

public:
	NewGameMenu(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_content(p_name + L"/Content", this),
		_commandPanel(p_name + L"/CommandPanel", this),
		_messageBox(p_name + L"/MessageBox", this)
	{
		_onActivationContract = addActivationCallback([&](){
			_content.clear();
			_messageBox.deactivate();
		});

		_layout.addWidget(&_content, spk::Layout::SizePolicy::Extend);
		_layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);

		spk::SafePointer<spk::PushButton> confirmButton = _commandPanel.addButton(L"ConfirmButton", L"Confirm");
		WidgetAddons::ApplyFormat(confirmButton);
		
		spk::SafePointer<spk::PushButton> cancelButton = _commandPanel.addButton(L"CancelButton", L"Cancel");
		WidgetAddons::ApplyFormat(cancelButton);

		_messageBox.setLayer(100);
		_confirmContract = _commandPanel.subscribe(L"ConfirmButton", [&]()
		{
			_tryLaunchGame();
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