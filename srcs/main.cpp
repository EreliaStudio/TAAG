#include <sparkle.hpp>

#include "asset_atlas.hpp"
#include "widget_override.hpp"

class HomeScreen : public spk::Screen
{
private:
	class BackgroundRenderer : public spk::Widget
	{
	private:
		void _onGeometryChange() override
		{

		}

	public:
		BackgroundRenderer(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
			spk::Widget(p_name, p_parent)
		{

		}
	};

	class ConnectionPanel : public spk::Widget
	{
	private:
		class ConnectionWidget : public spk::Widget
		{
		private:
			using UsernameRow = FormRow<TextEdit>;
			using PasswordRow = FormRow<TextEdit>;

			spk::FormLayout _layout;

			UsernameRow _usernameRow;
			PasswordRow _passwordRow;

			void _onGeometryChange() override
			{
				spk::cout << "Receive a size of " << geometry().size << " for " << name() << std::endl;
				spk::Geometry2D layoutGeometry = {{0, 0}, geometry().size};
				_layout.setGeometry(layoutGeometry);
			}

		public:
			ConnectionWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
				spk::Widget(p_name, p_parent),
				_usernameRow(p_name + L"/Username", this),
				_passwordRow(p_name + L"/Password", this)
			{
				_usernameRow.label.setText(L"Username:");
				_usernameRow.field.setPlaceholder(L"");
				_usernameRow.label.setMaximalSize({_usernameRow.label.maximalSize().x, 40});
				_usernameRow.field.setMaximalSize({_usernameRow.field.maximalSize().x, 40});
				
				_passwordRow.label.setText(L"Password:");
				_passwordRow.field.setPlaceholder(L"");
				_passwordRow.field.setObscured(true);
				_passwordRow.label.setMaximalSize({_passwordRow.label.maximalSize().x, 40});
				_passwordRow.field.setMaximalSize({_passwordRow.field.maximalSize().x, 40});

				_layout.setElementPadding({5, 5});
				_layout.addRow(&_usernameRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);
				_layout.addRow(&_passwordRow, spk::Layout::SizePolicy::Minimum, spk::Layout::SizePolicy::HorizontalExtend);

				_usernameRow.activate();
				_passwordRow.activate();
			}

			const std::wstring& username() const
			{
				return _usernameRow.field.text();
			}

			const std::wstring& password() const
			{
				return _passwordRow.field.text();
			}

			spk::Vector2UInt minimalSize() const override
			{
				spk::Vector2UInt usernameSize = _usernameRow.minimalSize();
				spk::Vector2UInt passwordSize = _passwordRow.minimalSize();

				return {std::max(usernameSize.x, passwordSize.x), usernameSize.y + passwordSize.y + _layout.elementPadding().y};
			}

			spk::Vector2UInt maximalSize() const override
			{
				const uint32_t maxVal = std::numeric_limits<uint32_t>::max();
				spk::Vector2UInt usernameSize = _usernameRow.maximalSize();
				spk::Vector2UInt passwordSize = _passwordRow.maximalSize();

				uint32_t width;
				if (usernameSize.x == maxVal || passwordSize.x == maxVal)
				{
					width = maxVal;
				}
				else
				{
					width = std::max(usernameSize.x, passwordSize.x);
				}

				uint32_t height;
				if (usernameSize.y == maxVal || passwordSize.y == maxVal)
				{
					height = maxVal;
				}
				else
				{
					height = usernameSize.y + passwordSize.y + _layout.elementPadding().y;
				}

				spk::cout << "Set maximal size of [" << name() << "] to " << spk::Vector2UInt(width, height) << std::endl;

				return {width, height};
			}
		};

		Frame _backgroundFrame;

		spk::VerticalLayout _layout;
		TextLabel _titleLabel;
		ConnectionWidget _connectionWidget;
		CommandPanel _commandPanel;

		void _onGeometryChange() override
		{
			spk::Geometry2D backgroundGeometry = {{0, 0}, geometry().size};
			_backgroundFrame.setGeometry(backgroundGeometry);

			_layout.setGeometry(backgroundGeometry.shrink(_backgroundFrame.cornerSize()));
		}
		
	public:
		ConnectionPanel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
			spk::Widget(p_name, p_parent),
			_backgroundFrame(p_name + L"/BackgroundFrame", this),
			_titleLabel(p_name + L"/TitleLabel", this),
			_connectionWidget(p_name + L"/ConnectionWidget", this),
			_commandPanel(p_name + L"/CommandPanel", this)
		{
			_backgroundFrame.setCornerSize({16, 16});

			_titleLabel.setText(L"Tactical Action Arena Game");
			_titleLabel.setMaximalSize({std::numeric_limits<uint32_t>::max(), 150});

			_commandPanel.setSizePolicy(spk::Layout::SizePolicy::Extend);
			_commandPanel.addButton(L"ConnectButton", L"Connect", [&](){
				spk::cout << "Connect user [" << _connectionWidget.username() << "] with password [" << _connectionWidget.password() << "]" << std::endl;	
			});
			_commandPanel.addButton(L"RegisterButton", L"Register", [&](){
				spk::cout << "Register user [" << _connectionWidget.username() << "] with password [" << _connectionWidget.password() << "]" << std::endl;	
			});

			_layout.setElementPadding({10, 10});
			_layout.addWidget(&_titleLabel, spk::Layout::SizePolicy::Extend);
			_layout.addWidget(&_connectionWidget, spk::Layout::SizePolicy::Minimum);
			_layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);

			_backgroundFrame.activate();
			_titleLabel.activate();
			_connectionWidget.activate();
			_commandPanel.activate();
		}

		spk::Vector2UInt minimalSize() const override
		{
			spk::Vector2UInt layoutSize = _layout.minimalSize();

			return (layoutSize + _backgroundFrame.cornerSize() * 2);
		}
	};

	BackgroundRenderer _backgroundRenderer;
	ConnectionPanel _connectionPanel;

	void _onGeometryChange() override
	{
		_backgroundRenderer.setGeometry({0, geometry().size});
		WidgetAddons::centerInParent(&_connectionPanel, _connectionPanel.minimalSize(), geometry());
	}

public:
	HomeScreen(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Screen(p_name, p_parent),
		_backgroundRenderer(p_name + L"/BackgroundRenderer", this),
		_connectionPanel(p_name + L"/ConnectionPanel", this)
	{
		_backgroundRenderer.activate();
		_connectionPanel.activate();
	}
};

class MainWidget : public spk::Widget
{
private:
	HomeScreen _homeScreen;

	void _onGeometryChange() override
	{
		_homeScreen.setGeometry({0, geometry().size});
	}

public:
	MainWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Widget(p_name, p_parent),
		_homeScreen(p_name + L"/HomeScreen", this)
	{
		_homeScreen.activate();
	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> win = app.createWindow(L"TAAG", {{0, 0}, {800, 600}});

	MainWidget mainWidget(L"MainWidget", win->widget());
	mainWidget.setGeometry(win->geometry());
	mainWidget.activate();

	return (app.run());
}