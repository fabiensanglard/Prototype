#include "SpriteEffect.h"
#include "UTIL_openGL.h"
#include "UTIL_Misc.h"
#include "Sprite.h"
#include "Engine.h"
SpriteEffect::SpriteEffect()
{
	bAlignCenter = true;
	bIsActive = true;
	mpTarget = NULL;//if this is non null, the effect will remain with this vector, and mPos becomes an offset
}
//================================================================================================//
						/********************
						** Spawn an effect **	
						*********************/
//================================================================================================//
void SpriteEffect::Spawn(Vec2 pos, Vec2 size, SpriteSheet sprite, float duration, bool glow, Vec2* target, bool playOnce)
{
	bIsActive = true;
	frame =0;
	mPos = oPos = pos;
	mSize = size;
	pSpr = sprite;
	fDuration = duration;
	bGlow = glow;
	mpTarget = target;
	if(mpTarget)
		oPos = *mpTarget + mPos;
	bPlayOnce = playOnce;
}
//================================================================================================//
						/********************
						** Update an effect **	
						*********************/
//================================================================================================//
void SpriteEffect::Update()
{
	if(mpTarget)
		oPos = *mpTarget+mPos;

	if(bPlayOnce)
	{
		int f = (int)floor(frame);
		if(f>=(pSpr.numFrames-1))
		{
			bIsActive = false;
			return;
		}
	}
	frame+=fDuration;
	if(!bPlayOnce)
		if(frame>(pSpr.numFrames-1))
			frame-=(pSpr.numFrames-1);
}
//================================================================================================//
						/*********************
						** Render an effect **	
						**********************/
//================================================================================================//
void SpriteEffect::Draw(const float interp)
{
	if(!bGlow)
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	else
		UTIL_GL::SetBlend(GL_ONE,GL_ONE);
//	glColor4f(1,1,1,1);

	Vec2 p;
	if(mpTarget == NULL)
		p = UTIL_Misc::Interpolate(mPos,oPos,interp);
	else
		p = UTIL_Misc::Interpolate(*mpTarget+mPos,oPos,interp);

	if(bAlignCenter)
		RenderSprite(pSpr,(int)frame,p.x-(mSize.x*0.5f),p.y-(mSize.y*0.5f),mSize.x,mSize.y);
	else
		RenderSprite(pSpr,(int)frame,p.x,p.y,mSize.x,mSize.y);

}

void SpriteEffect::LoadFromFile(CFileIO &fIO)
{
}
void SpriteEffect::WriteToFile(CFileIO &fIO)
{
}
