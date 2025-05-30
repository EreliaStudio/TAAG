#pragma once

#include <sparkle.hpp>

#include "asset_atlas.hpp"

class WidgetAddons
{
private:
	static inline AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
	static inline spk::Font::Size defaultFontSize = {16, 5};
	static inline spk::Color defaultFontColor = spk::Color::white;
	static inline spk::Color defaultOutlineColor = spk::Color::black;
	
	template<typename T>
	static void centerInParent(T* p_object, const spk::Vector2UInt& p_childSize, const spk::Geometry2D& p_parentGeometry)
	{
		spk::Vector2Int widgetAnchor = (p_parentGeometry.size - p_childSize) / 2;
		p_object->setGeometry({widgetAnchor, p_childSize});
	}

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
        p_widget->setFontColor(defaultFontColor, defaultOutlineColor);
        p_widget->setFontSize(defaultFontSize);
    }

    static void ApplyFormat(spk::SafePointer<spk::TextLabel> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
		p_widget->setTextAlignment(spk::HorizontalAlignment::Centered, spk::VerticalAlignment::Centered);
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(defaultFontColor, defaultOutlineColor);
        p_widget->setFontSize(defaultFontSize);
    }

    static void ApplyFormat(spk::SafePointer<spk::TextEdit> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(defaultFontColor, defaultOutlineColor);
        p_widget->setFontSize(defaultFontSize);
    }

    static void ApplyFormat(spk::SafePointer<spk::TextArea> p_widget)
    {
        p_widget->setNineSlice(AssetAtlas::instance()->spriteSheet(L"defaultNineSlice"));
        p_widget->setFont(AssetAtlas::instance()->font(L"defaultFont"));
        p_widget->setFontColor(defaultFontColor, defaultOutlineColor);
        p_widget->setFontSize(defaultFontSize);
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

class Frame : public spk::Frame
{
private:

public:
	Frame(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
		spk::Frame(p_name, p_parent)
	{
		WidgetAddons::ApplyFormat(this);
	}
};

class CommandPanel : public spk::CommandPanel
{
public:
	struct Descriptor
	{
		std::wstring text;
		spk::PushButton::Job job;
	};

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

	CommandPanel(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent, std::initializer_list<Descriptor> descriptors) :
		spk::CommandPanel(p_name, p_parent)
	{
		for (const auto& descriptor : descriptors)   
		{
			addButton(p_name + L"/" + descriptor.text + L"Button", descriptor.text, descriptor.job);
		}
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

    std::map<std::wstring, ButtonData> _instanciedButtons;

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

        _instanciedButtons.emplace(p_name, ButtonData{
            newButton,
            newButton->subscribe(p_job)
        });

        return newButton;
    }

	void setButtonCallback(const std::wstring& p_buttonName, const spk::PushButton::Job& p_job)
	{
		if (_instanciedButtons.contains(p_buttonName) == false)
		{
			throw std::runtime_error("Button not found");
		}
		
		auto& buttonHolder = _instanciedButtons[p_buttonName];

		buttonHolder.contract = buttonHolder.button->subscribe(p_job);
	}

	void show(const std::wstring& p_text)
    {
        setText(p_text);
		activate();
    }
};

class InformationMessageBox : public spk::InformationMessageBox
{
public:
    InformationMessageBox(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
        spk::InformationMessageBox(p_name, p_parent)
    {
		WidgetAddons::ApplyFormat(this);

		WidgetAddons::ApplyFormat(&(textArea()));

		WidgetAddons::ApplyFormat(button());
    }

	void setText(const std::wstring& p_text) override
	{
		spk::MessageBox::setText(p_text);
		setMinimalWidth(commandPanel().minimalSize().x * 2);
		parent()->requireGeometryUpdate();
	}

	void show(const std::wstring& p_text)
    {
        setText(p_text);
		activate();
    }
};

class RequestMessageBox : public spk::RequestMessageBox
{
private:

public:
    RequestMessageBox(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
        spk::RequestMessageBox(p_name, p_parent)
    {
		WidgetAddons::ApplyFormat(this);

		WidgetAddons::ApplyFormat(&(textArea()));

		WidgetAddons::ApplyFormat(firstButton());
		WidgetAddons::ApplyFormat(secondButton());
    }

	void setText(const std::wstring& p_text) override
	{
		spk::MessageBox::setText(p_text);
		setMinimalWidth(commandPanel().minimalSize().x * 2);
		parent()->requireGeometryUpdate();
	}

	void show(const std::wstring& p_text)
    {
        setText(p_text);
		activate();
    }
};