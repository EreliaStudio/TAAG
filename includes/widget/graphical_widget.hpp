#pragma once

#include <sparkle.hpp>

class GraphicalWidget : public spk::Widget
{
private:
	static inline spk::Vector2Int _nodeSize = {16, 16};

protected:
	spk::Vector2 convertScreenToWorldPosition(const spk::Vector2Int& p_screenPosition);
	spk::Vector2Int convertWorldToScreenPosition(const spk::Vector2& p_worldPosition);
	spk::Vector2Int convertWorldToScreenPosition(const spk::Vector3& p_worldPosition);

public:
	GraphicalWidget(const std::wstring& p_name, spk::SafePointer<spk::Widget> p_parent);

	static void setNodeSize(const spk::Vector2Int& p_nodeSize);
};