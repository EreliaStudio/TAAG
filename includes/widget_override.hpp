#pragma once

#include <sparkle.hpp>

#include "asset_atlas.hpp"

class WidgetAddons
{
private:
	static inline AssetAtlas::Instanciator _assetAtlasInstanciator;

public:
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
