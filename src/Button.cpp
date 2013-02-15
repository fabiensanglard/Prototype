#include "Button.h"

Button::Button(void)
{
	state = RELEASED;
}

Button::~Button(void)
{
}
void Button::Update(bool status)
{
	if(status)
	{
		if(state == RELEASED || state == JUST_RELEASED)
			state = JUST_PRESSED;
	}
	else
	{
		if(state == PRESSED || state == JUST_PRESSED)
			state = JUST_RELEASED;
	}
}
void Button::PostUpdate()
{
	if(state==JUST_PRESSED)
		state = PRESSED;
	else if(state==JUST_RELEASED)
		state = RELEASED;
}