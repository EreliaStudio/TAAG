#pragma once

#include <sparkle.hpp>

#include "widget/graphical_widget.hpp"

#include "structure/actor_map.hpp"

#include "renderer/shape_renderer.hpp"

class ActorListManager : public GraphicalWidget
{
private:
	std::unordered_map<Actor::Shape, std::unique_ptr<ShapeRenderer>> _shapeRenderers;

	void _requestActorList(const std::vector<ActorMap::ActorID>& p_actorIDs);
	void _parseActorData(const spk::Message& p_message);
	void _parseActorList(const spk::Message& p_message);
	void _parseActorRemoval(const spk::Message& p_message);

	void _onGeometryChange();
	void _onPaintEvent(spk::PaintEvent& p_event) override;

	void _onUpdateEvent(spk::UpdateEvent& p_event) override;

public:
	ActorListManager(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);
};