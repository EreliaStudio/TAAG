#pragma once

#include "actor.hpp"

#include "action_slot.hpp"

class Player : public Actor
{
public:
	static inline const size_t NB_ACTION_SLOTS = 6;
private:
	ActionSlot _actionSlots[NB_ACTION_SLOTS];

public:
	Player();

	void setAction(size_t p_index, std::unique_ptr<Action>&& p_action);
	ActionSlot& actionSlot(size_t p_index);
};