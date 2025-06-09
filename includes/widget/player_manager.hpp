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

	spk::Timer _motionRequestTimer = spk::Timer(spk::Duration(16, spk::TimeUnit::Millisecond));
	spk::Vector2 _controllerDirection;
	bool _motionRequests[4];

	void _updatePlayerUBOs();
	void _parsePlayerIDAssignation(const spk::Message& p_message);

	void _requestPlayerMotion(const spk::Vector2& p_direction);
	void _requestPlayerRotation(const float& p_rotation);

	void _onGeometryChange() override;
	void _onPaintEvent(spk::PaintEvent& p_event) override;

	void _onKeyboardEvent(spk::KeyboardEvent& p_event) override;
	void _onMouseEvent(spk::MouseEvent& p_event) override;
	void _onControllerEvent(spk::ControllerEvent& p_event) override;

	void _onUpdateEvent(spk::UpdateEvent& p_event) override;

public:
	PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};