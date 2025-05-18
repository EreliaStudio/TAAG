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
        p_widget->setFontColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

    static void ApplyFormat(spk::SafePointer<spk::TextLabel> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

    static void ApplyFormat(spk::SafePointer<spk::TextEdit> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

    static void ApplyFormat(spk::SafePointer<spk::TextArea> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(spk::Color(255, 255, 255), spk::Color(0, 0, 0));
        p_widget->setFontSize({16, 3});
    }

	static void ApplyFormat(spk::SafePointer<spk::IInterfaceWindow::MenuBar> p_menuBar)
	{
        ApplyFormat(p_menuBar->titleLabel());

        ApplyFormat(p_menuBar->minimizeButton());
        ApplyFormat(p_menuBar->maximizeButton());
        ApplyFormat(p_menuBar->closeButton());
	}

	static void ApplyFormat(spk::SafePointer<spk::IInterfaceWindow> p_interfaceWindow)
	{
        ApplyFormat(p_interfaceWindow->menuBar());

        ApplyFormat(p_interfaceWindow->backgroundFrame());
        ApplyFormat(p_interfaceWindow->minimizedBackgroundFrame());
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
private:
	struct ButtonData
    {
        spk::SafePointer<spk::PushButton> button;
        spk::PushButton::Contract         contract;
    };

    std::vector<ButtonData> _buttonContainer;

	using spk::CommandPanel::addButton;

public:
	CommandPanel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::CommandPanel(p_name, p_parent)
	{

	}

    spk::SafePointer<spk::PushButton> addButton(const std::wstring& p_name, const std::wstring& p_text, const spk::PushButton::Job& p_job)
    {
        spk::SafePointer<spk::PushButton> newButton = spk::CommandPanel::addButton(p_name, p_text);

        WidgetAddons::ApplyFormat(newButton);

		_buttonContainer.emplace_back(ButtonData{
            newButton,
            newButton->subscribe(p_job)
        });

        return newButton;
    }
};

template<typename WidgetType>
struct FormRow : public spk::FormLayout::Row<WidgetType>
{
	FormRow(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::FormLayout::Row<WidgetType>(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(&(this->label));
	}
};

class MessageBox : public spk::MessageBox
{
private:
	struct ButtonData
    {
        spk::SafePointer<spk::PushButton> button;
        spk::PushButton::Contract         contract;
    };

    std::vector<ButtonData> _instanciedButtons;

	using spk::MessageBox::addButton;

public:
	MessageBox(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) : 
		spk::MessageBox(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(this);

		WidgetAddons::ApplyFormat(&(textArea()));

		setMenuHeight(25);
		setTitle(L"Unnamed message box");
	}

	void setTitle(const std::wstring& p_title)
	{
		menuBar()->titleLabel()->setText(p_title);
	}

	void setText(const std::wstring& p_text) override
	{
		spk::MessageBox::setText(p_text);
		setMinimalWidth(commandPanel().minimalSize().x * 2);
		parent()->requireGeometryUpdate();
	}

	spk::SafePointer<spk::PushButton> addButton(const std::wstring& p_name, const std::wstring& p_label, const spk::PushButton::Job& p_job)
    {
        auto newButton = commandPanel().addButton(p_name, p_label);

        WidgetAddons::ApplyFormat(newButton);

        _instanciedButtons.emplace_back(ButtonData{
            newButton,
            newButton->subscribe(p_job)
        });

        return newButton;
    }
};