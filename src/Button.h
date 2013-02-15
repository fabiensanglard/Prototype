#pragma once

#define JUST_PRESSED	0
#define PRESSED			1
#define RELEASED		2
#define JUST_RELEASED	3
class Button
{
public:
	int state;
	int BoundKey;	// so the user can change the key configuration

	Button(void);
	~Button(void);
	void Update(bool status);
	void PostUpdate();
};
