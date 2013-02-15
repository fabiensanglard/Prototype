#include "Explosion.h"
#include "Sprite.h"
//================================================================================================//
						/********************
						** explosion class **	
						*********************/
//================================================================================================//
Explosion::Explosion(SpriteSheet spr, Vec2 p, float s, float d, float r, bool glow)
{
	IsActive = true;
	pSpr = spr;
	Pos = p;
	Size = s/2;
	fDuration = d;//(float)((float)pSpr.numFrames/d);
	frame = 0;
	rotation = r;
	bGlow = glow;
}
void Explosion::Update()
{
	if(frame<pSpr.numFrames-1)
		frame += fDuration;
	else
		IsActive = false;
}
void Explosion::Render(const float interp)
{
	if(!IsActive)
		return;

	glColor4f(1,1,1,1);
	if(bGlow)
		UTIL_GL::SetBlend(GL_ONE, GL_ONE);
	else
		UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	RenderSprite(pSpr, (int)frame, Pos.x-(Size/2), Pos.y-(Size/2), Size, Size);
	RenderRotatedSprite(pSpr, (int)frame, Pos.x, Pos.y, Size, Size, rotation);
}