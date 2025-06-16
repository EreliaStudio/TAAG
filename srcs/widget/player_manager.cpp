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

void PlayerManager::_requestPlayerRotation(const float& p_rotation)
{
	spk::Message request = spk::Message(static_cast<spk::Message::Header::Type>(MessageType::PlayerRotationRequest));

	request << p_rotation;

	Context::instance()->client.send(request);
}

void PlayerManager::_onUpdateEvent(spk::UpdateEvent& p_event)
{
	_playerMotionDirection = 0;

	for (auto& [event, action] : _actions)
	{
		if (action->isInitialized() == false)
		{
			action->initialize(p_event);
		}
		
        action->update();
	}

	if (_playerMotionDirection != 0)
	{
		_requestPlayerMotion(_playerMotionDirection.normalize());
	}
}

PlayerManager::PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent),
	_cameraUBO(UniformBufferObjectAtlas::instance()->ubo(L"cameraData"))
{
	Context::instance()->client.subscribe(MessageType::PlayerIDAssignation, [&](const spk::Message& p_message) {
		_parsePlayerIDAssignation(p_message);
	}).relinquish();

	EventDispatcher::subscribe(Event::PlayerMotion, [&](){
		_updatePlayerUBOs();
	}).relinquish();

	_actions[ActionType::MotionUp] = std::make_unique<KeyboardAction>(spk::Keyboard::Z, spk::InputState::Down, 16, [&](){
				_playerMotionDirection += spk::Vector2Int(0, 1);
			});

	_actions[ActionType::MotionDown] = std::make_unique<KeyboardAction>(spk::Keyboard::S, spk::InputState::Down, 16, [&](){
				_playerMotionDirection += spk::Vector2Int(0, -1);
			});

	_actions[ActionType::MotionRight] = std::make_unique<KeyboardAction>(spk::Keyboard::D, spk::InputState::Down, 16, [&](){
				_playerMotionDirection += spk::Vector2Int(1, 0);
			});

	_actions[ActionType::MotionLeft] = std::make_unique<KeyboardAction>(spk::Keyboard::Q, spk::InputState::Down, 16, [&](){
				_playerMotionDirection += spk::Vector2Int(-1, 0);
			});

	_actions[ActionType::Rotation] = std::make_unique<MouseMotionAction>(
				MouseMotionAction::Mode::Absolute,
				[&](const spk::Vector2Int& pos){
					const auto center = geometry().size / 2 + geometry().anchor;
					
					spk::Vector2Int delta = pos - center;
					
					float angle = std::atan2(-float(delta.y), float(delta.x));

					if (angle < 0)
					{
						angle += 2 * float(M_PI);
					}

					_requestPlayerRotation(angle);
				}
			);

	_actions[ActionType::ActionSlotA] = std::make_unique<MouseButtonAction>(spk::Mouse::Button::Left, spk::InputState::Down, 16, [&](spk::SafePointer<const spk::Mouse> p_mouse){
				const auto center = geometry().size / 2 + geometry().anchor;
					
				spk::Vector2 delta = static_cast<spk::Vector2>(p_mouse->position - center).n;
			});
}