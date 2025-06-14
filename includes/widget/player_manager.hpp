#pragma once

#include <sparkle.hpp>

#include "widget/graphical_widget.hpp"

#include "structure/action.hpp"

class PlayerManager : public GraphicalWidget
{
private:
	enum class ActionType
	{
		MotionUp,
		MotionDown,
		MotionLeft,
		MotionRight,
		Rotation
	};
	UniformBufferObjectAtlas::Instanciator _uniformBufferObjectAtlasInstanciator;

	spk::Vector2Int _screenRenderDimension;
	spk::OpenGL::UniformBufferObject& _cameraUBO;
	Actor::Contract _onPlayerEditionContract;

	spk::Camera _camera;
	
	spk::Vector2 _playerMotionDirection;

	std::unordered_map<ActionType, std::unique_ptr<Action>> _actions;

	void _updatePlayerUBOs();
	void _parsePlayerIDAssignation(const spk::Message& p_message);

	void _requestPlayerMotion(const spk::Vector2& p_direction);
	void _requestPlayerRotation(const float& p_rotation);

	void _onGeometryChange() override;
	void _onPaintEvent(spk::PaintEvent& p_event) override;

	void _onUpdateEvent(spk::UpdateEvent& p_event) override;

public:
	PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};