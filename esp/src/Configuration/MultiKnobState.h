//
// Created by Joel Neumann on 03.07.24.
//

#ifndef DREA_V2_MULTI_KNOB_STATE_H
#define DREA_V2_MULTI_KNOB_STATE_H


class MultiKnobState
{
public:
	enum State
	{
		OFF,
		ON
	};

	State getState() const;

	void setState(State state);

private:
	State state_ = ON;
};

#endif//DREA_V2_MULTI_KNOB_STATE_H
