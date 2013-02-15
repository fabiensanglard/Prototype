#pragma once
#include "Vector.h"
#include "SpriteSheet.h"

class Explosion
{
public:
	bool IsActive;
	Vec2 Pos;
	float Size;
	float frame;
	float rotation;
	float fDuration;
	bool bGlow;
	SpriteSheet pSpr;

	Explosion(SpriteSheet spr, Vec2 p, float s, float d, float r, bool glow);
	void Update();
	void Render(const float interp);
};