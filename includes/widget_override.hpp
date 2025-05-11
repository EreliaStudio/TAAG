#pragma once

#include <sparkle.hpp>

#include "asset_atlas.hpp"

class WidgetAddons
{
private:
	static inline AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
	static void ApplyFormat(spk::SafePointer<spk::Frame> p_widget)
	{
		p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
	}

    static void ApplyFormat(spk::SafePointer<spk::PushButton> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"), spk::PushButton::State::Released);
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice_Dark"), spk::PushButton::State::Pressed);
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice_Light"), spk::PushButton::State::Hovered);

        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

    static void ApplyFormat(spk::SafePointer<spk::TextLabel> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

    static void ApplyFormat(spk::SafePointer<spk::TextEdit> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setTextColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

	static void ApplyFormat(spk::SafePointer<spk::IInterfaceWindow::MenuBar> p_menuBar)
	{
        ApplyFormat(p_menuBar->titleLabel());

        ApplyFormat(p_menuBar->minimizeButton());
        ApplyFormat(p_menuBar->maximizeButton());
        ApplyFormat(p_menuBar->closeButton());
	}

	static void ApplyFormat(spk::SafePointer<spk::IInterfaceWindow> p_window)
    {
        p_window->setCornerSize({4, 4});
        p_window->setMenuHeight(24.f);

    	ApplyFormat(p_window->backgroundFrame());

        ApplyFormat(p_window->menuBar());
    }
};

class PushButton : public spk::PushButton
{
private:
	spk::PushButton::Contract _onClickContract;

public:
	PushButton(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::PushButton(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(this);
	}

	void setOnClick(const std::function<void()>& p_callback)
	{
		_onClickContract = subscribe(p_callback);
	}
};

class TextLabel : public spk::TextLabel
{
private:

public:
	TextLabel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextLabel(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(this);
	}
};

class TextEdit : public spk::TextEdit
{
private:

public:
	TextEdit(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::TextEdit(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(this);
	}
};

class CommandPanel : public spk::CommandPanel
{
public:
    using spk::CommandPanel::CommandPanel;

	CommandPanel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::CommandPanel(p_name, p_parent)
	{

	}

    spk::SafePointer<spk::PushButton> addButton(const std::wstring& p_name, const std::wstring& p_text) override
    {
        spk::SafePointer<spk::PushButton> newButton = spk::CommandPanel::addButton(p_name, p_text);

        if (newButton != nullptr)
		{
            WidgetAddons::ApplyFormat(newButton);
		}
        return newButton;
    }
};

template <typename TContentType>
class InterfaceWindow : public spk::InterfaceWindow<TContentType>
{
public:
    InterfaceWindow(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::InterfaceWindow<TContentType>(p_name, p_parent)
    {
        WidgetAddons::ApplyFormat(this);
    }
};

class MessagePopupContent : public spk::Widget
{
private:
	spk::VerticalLayout _layout;

	TextLabel _lineLabels[4];
	spk::SpacerWidget _spacer;
	CommandPanel _commandPanel;

	void _onGeometryChange()
	{
		_layout.setGeometry({0, geometry().size});
	}

public:
	MessagePopupContent(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) : 
		spk::Widget(p_name, p_parent),
		_lineLabels{
			TextLabel(p_name + L"/LineLabel A", this),
			TextLabel(p_name + L"/LineLabel B", this),
			TextLabel(p_name + L"/LineLabel C", this),
			TextLabel(p_name + L"/LineLabel D", this),
		},
		_spacer(p_name + L"/Spacer", this),
		_commandPanel(p_name + L"/CommandPanel", this)
	{
		_layout.setElementPadding(0);
		for (size_t i = 0; i < 4; i++)
		{
			_lineLabels[i].setNineSlice(nullptr);
			_lineLabels[i].setTextAlignment(spk::HorizontalAlignment::Centered, spk::VerticalAlignment::Centered);
			_lineLabels[i].activate();

			_layout.addWidget(&_lineLabels[i], spk::Layout::SizePolicy::Minimum);
		}

		_layout.addWidget(&_spacer, spk::Layout::SizePolicy::Extend);

		_commandPanel.activate();
		
		_layout.addWidget(&_commandPanel, spk::Layout::SizePolicy::Minimum);
	}

	spk::SafePointer<spk::PushButton> addButton(const std::wstring& p_buttonName, const std::wstring& p_buttonText)
	{
		return (_commandPanel.addButton(p_buttonName, p_buttonText));
	}

	void setLineText(size_t p_line, const std::wstring& p_text)
	{
		_lineLabels[p_line].setText(p_text);
		requireGeometryUpdate();
	}

	spk::Vector2UInt minimalSize() const override
    {
        const uint32_t padY = _layout.elementPadding().y;

        uint32_t maxWidth   = 0;
        uint32_t totalHeight = 0;
        uint32_t childCount  = 0;

        for (const auto& lbl : _lineLabels)
        {
            const spk::Vector2UInt sz = lbl.minimalSize();
            maxWidth    = std::max(maxWidth,  sz.x);
            totalHeight += sz.y;
            ++childCount;
        }

        {
            const spk::Vector2UInt sz = _commandPanel.minimalSize();
            maxWidth    = std::max(maxWidth,  sz.x);
            totalHeight += sz.y;
            ++childCount;
        }

        if (childCount > 1)
            totalHeight += padY * (childCount - 1);

        return { maxWidth, totalHeight };
    }
};

class MessagePopup : public InterfaceWindow<MessagePopupContent>
{
public:
	
private:
	spk::SafePointer<spk::PushButton> _closeButton;
	spk::PushButton::Contract _closeButtonContract;
	spk::ContractProvider::Contract _closeMenuBarContract;

public:
	MessagePopup(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) : 
		InterfaceWindow(p_name, p_parent)
	{
		_closeMenuBarContract = subscribeTo(spk::IInterfaceWindow::Event::Close, [&](){close();});

		menuBar()->titleLabel()->setText(L"Message box");
		_closeButton = content()->addButton(L"CloseButton", L"Close");
		WidgetAddons::ApplyFormat(_closeButton);

		_closeButtonContract = _closeButton->subscribe([&](){close();});
	}

	void setLineText(size_t p_line, const std::wstring& p_text)
	{
		content()->setLineText(p_line, p_text);
		setMinimumContentSize(content()->minimalSize());
	}
};