#pragma once

#include <sparkle.hpp>

#include "asset_atlas.hpp"

class PushButton : public spk::PushButton
{
private:
	AssetAtlas::Instanciator _assetAtlasInstanciator;

	spk::PushButton::Contract _onClickContract;

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

	void setOnClick(const std::function<void()>& p_callback)
	{
		_onClickContract = subscribe(p_callback);
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