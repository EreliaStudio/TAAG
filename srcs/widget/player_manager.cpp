#include "widget/player_manager.hpp"

#include "structure/context.hpp"

void PlayerManager::_updatePlayerUBOs()
{
	const spk::Vector2 playerPos = Context::instance()->player->position();

	const spk::Vector3 eye    { playerPos.x, playerPos.y, 20.0f };
	const spk::Vector3 center { playerPos.x, playerPos.y, 0.0f };
	const spk::Vector3 up     { 0.0f, 1.0f, 0.0f };

	const spk::Matrix4x4 view = spk::Matrix4x4::lookAt(eye, center, up);
	const spk::Matrix4x4 proj = _camera.projectionMatrix();

	_cameraUBO[L"view"] = view;
	_cameraUBO[L"proj"] = proj;
	_cameraUBO.validate();
}

void PlayerManager::_parsePlayerIDAssignation(const spk::Message& p_message)
{
	try
	{
		Context::instance()->playerID = p_message.get<ActorMap::ActorID>();
		if (Context::instance()->actorMap.contains(Context::instance()->playerID) == false)
		{
			Context::instance()->actorMap.addActor(Context::instance()->playerID, std::make_unique<Actor>());
		}

		Context::instance()->player = Context::instance()->actorMap.actor(Context::instance()->playerID);

		_onPlayerEditionContract = Context::instance()->player->subscribeToEdition([&](){
			_updatePlayerUBOs();
		});
		_onPlayerEditionContract.trigger();
	}
	catch (const std::exception& e)
	{
		PROPAGATE_ERROR("Error while deserializing player ID", e);
	}
}

void PlayerManager::_onGeometryChange()
{
	_screenRenderDimension = convertScreenToWorldPosition(geometry().size) - convertScreenToWorldPosition({0, 0});
	
	_camera.setOrthographic(-_screenRenderDimension.x / 2, _screenRenderDimension.x / 2, -_screenRenderDimension.y / 2, _screenRenderDimension.y / 2, 0, 100);

	_updatePlayerUBOs();
}

void PlayerManager::_onPaintEvent(spk::PaintEvent& p_event)
{
	
}

void PlayerManager::_requestPlayerMotion(const spk::Vector2& p_direction)
{
	spk::Message request = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::PlayerMotionRequest));

	request << p_direction;

	Context::instance()->client.send(request);
}

void PlayerManager::_onKeyboardEvent(spk::KeyboardEvent& p_event)
{
	if (p_event.type == spk::KeyboardEvent::Type::Press)
	{
		if (p_event.key == spk::Keyboard::Z)
		{
			_motionRequests[0] = true;
		}
		if (p_event.key == spk::Keyboard::S)
		{
			_motionRequests[1] = true;
		}
		if (p_event.key == spk::Keyboard::D)
		{
			_motionRequests[2] = true;
		}
		if (p_event.key == spk::Keyboard::Q)
		{
			_motionRequests[3] = true;
		}
	}
	else if (p_event.type == spk::KeyboardEvent::Type::Release)
	{
		if (p_event.key == spk::Keyboard::Z)
		{
			_motionRequests[0] = false;
		}
		if (p_event.key == spk::Keyboard::S)
		{
			_motionRequests[1] = false;
		}
		if (p_event.key == spk::Keyboard::D)
		{
			_motionRequests[2] = false;
		}
		if (p_event.key == spk::Keyboard::Q)
		{
			_motionRequests[3] = false;
		}
	}
}

void PlayerManager::_requestPlayerRotation(const float& p_rotation)
{
	spk::Message request = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::PlayerRotationRequest));

	request << p_rotation;

	Context::instance()->client.send(request);
}

void PlayerManager::_onMouseEvent(spk::MouseEvent& p_event)
{
	if (p_event.type == spk::MouseEvent::Type::Motion)
	{
		spk::Vector2Int centerOfScreen = geometry().size / 2 + geometry().anchor;
		spk::Vector2Int mousePosition = p_event.mouse->position;

		spk::Vector2Int delta = mousePosition - centerOfScreen;

		float angleRad = std::atan2(-static_cast<float>(delta.y), static_cast<float>(delta.x));

		if (angleRad < 0)
		{
			angleRad += std::numbers::pi * 2;
		}
			
		_requestPlayerRotation(angleRad);
	}
}

void PlayerManager::_onControllerEvent(spk::ControllerEvent& p_event)
{
	if (p_event.type == spk::ControllerEvent::Type::JoystickMotion)
	{
		if (p_event.joystick.id == spk::Controller::Joystick::Left)
		{	
			if (p_event.joystick.values.x >= 1000 || p_event.joystick.values.x <= -1000 ||
				p_event.joystick.values.y >= 1000 || p_event.joystick.values.y <= -1000)
			{
				_controllerDirection = p_event.joystick.values.normalize();
			}
			else
			{
				_controllerDirection = 0;
			}
		}
		else if (p_event.joystick.id == spk::Controller::Joystick::Right)
		{
			const auto& pos = p_event.joystick.values;

			if (pos.x >= 1000 || pos.x <= -1000 ||
				pos.y >= 1000 || pos.y <= -1000)
			{
				float angle = std::atan2(pos.y, pos.x);

				if (angle < 0.0f)
					angle += 2.0f * static_cast<float>(M_PI);

				_requestPlayerRotation(angle);
			}
		}
	}
}

void PlayerManager::_onUpdateEvent(spk::UpdateEvent& p_event)
{
	if (_motionRequestTimer.state() == spk::Timer::State::Running)
	{
		return ;
	}

	std::vector<spk::Vector2> directions = {
		{0, 1},
		{0, -1},
		{1, 0},
		{-1, 0}
	};
	spk::Vector2 direction = 0;

	bool needMotionRequest = false;

	for (size_t i = 0; i < 4; i++)
	{
		if (_motionRequests[i] == true)
		{
			direction += directions[i];
			needMotionRequest = true;
		}
	}

	if (needMotionRequest == true)
	{
		_requestPlayerMotion(direction);
	}

	if (_controllerDirection != 0)
	{
		_requestPlayerMotion(_controllerDirection);
	}
}

PlayerManager::PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent),
	_cameraUBO(UniformBufferObjectAtlas::instance()->ubo(L"cameraData"))
{
	Context::instance()->client.subscribe(MessageType::PlayerIDAssignation, [this](const spk::Message& p_message) {
		_parsePlayerIDAssignation(p_message);
	}).relinquish();

	EventDispatcher::subscribe(Event::PlayerMotion, [&](){
		_updatePlayerUBOs();
	}).relinquish();
}