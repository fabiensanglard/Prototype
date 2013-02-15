#pragma once
#include "Vector.h"
#include "SpriteSheet.h"
#include "FileIO.h"

class SpriteEffect
{
public:
	SpriteEffect();
	void Spawn(Vec2 pos, Vec2 size, SpriteSheet sprite, float duration, bool glow, Vec2* target, bool playOnce);
	void Update();
	void Draw(const float interp);

	bool bPlayOnce;
	bool bIsActive;
	SpriteSheet pSpr;
	float frame;
	Vec2 oPos;
	Vec2 mPos;
	Vec2 *mpTarget;
	Vec2 mSize;
	float fDuration;
	bool bGlow;
	bool bAlignCenter;//default 

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);

};