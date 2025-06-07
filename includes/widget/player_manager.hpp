#pragma once

#include <sparkle.hpp>

#include "widget/graphical_widget.hpp"

class PlayerManager : public GraphicalWidget
{
private:
	UniformBufferObjectAtlas::Instanciator _uniformBufferObjectAtlasInstanciator;

	spk::Vector2Int _screenRenderDimension;
	spk::OpenGL::UniformBufferObject& _cameraUBO;
	Actor::Contract _onPlayerEditionContract;

	spk::Camera _camera;

	void _updatePlayerUBOs();
	void _parsePlayerIDAssignation(const spk::Message& p_message);

	void _onGeometryChange();
	void _onPaintEvent(spk::PaintEvent& p_event);

public:
	PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};