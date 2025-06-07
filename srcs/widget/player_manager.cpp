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
	if (Context::instance()->playerID == -1 ||
		Context::instance()->actorMap.contains(Context::instance()->playerID) == false)
	{
		return ;
	}

	spk::SafePointer<Actor> player = Context::instance()->actorMap.actor(Context::instance()->playerID);
	static spk::Vector2 lastPosition = player->position() + 1;

	if (lastPosition != player->position())
	{
		lastPosition = player->position();
	}
}

PlayerManager::PlayerManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent) :
	GraphicalWidget(p_name, p_parent),
	_cameraUBO(UniformBufferObjectAtlas::instance()->ubo(L"cameraData"))
{
	Context::instance()->client.subscribe(MessageType::PlayerIDAssignation, [this](const spk::Message& p_message) {
		_parsePlayerIDAssignation(p_message);
	}).relinquish();
}