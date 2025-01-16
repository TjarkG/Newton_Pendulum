//
// Created by Joel Neumann on 10.07.24.
//

#include "MultiKnobState.h"

MultiKnobState::State MultiKnobState::getState() const
{
	return state_;
}

void MultiKnobState::setState(MultiKnobState::State state)
{
	state_ = state;
}
