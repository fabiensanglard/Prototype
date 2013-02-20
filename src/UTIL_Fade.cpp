#include "UTIL_Fade.h"
#include "UTIL_Misc.h"
#include "UTIL_openGL.h"

Fade::Fade()
{
	mDuration = 0;
	mStart = 0;
	srcColor[0]=srcColor[1]=srcColor[2]=srcColor[3]=0;
	dstColor[0]=dstColor[1]=dstColor[2]=dstColor[3]=0;
}
void Fade::SetSrcColor(float r, float g, float b, float a)
{
	srcColor[0]=r;
	srcColor[1]=g;
	srcColor[2]=b;
	srcColor[3]=a;
}
void Fade::SetDstColor(float r, float g, float b, float a)
{
	dstColor[0]=r;
	dstColor[1]=g;
	dstColor[2]=b;
	dstColor[3]=a;
}
void Fade::StartFade(float time, float duration)
{
	mDuration = duration;
	mStart = time;
}
void Fade::GetColor(float time, float* rgba)
{
	rgba[0] = UTIL_Misc::TimeRamp(srcColor[0],dstColor[0],mStart,mDuration,time);
	rgba[1] = UTIL_Misc::TimeRamp(srcColor[1],dstColor[1],mStart,mDuration,time);
	rgba[2] = UTIL_Misc::TimeRamp(srcColor[2],dstColor[2],mStart,mDuration,time);
	rgba[3] = UTIL_Misc::TimeRamp(srcColor[3],dstColor[3],mStart,mDuration,time);
}
bool Fade::GetState(float time)//updates
{
	if(time<=mStart+mDuration)
		return true;
	return false;
}
void Fade::UpdateFade(float time)
{
	if(GetState(time))
	{
		switch(mState)
		{
		case 0: mScale+=1.5f; break;
		case 1: mScale+=1.5f; break;
		}
	}	
}
void Fade::RenderFade(float time)
{
	if(GetState(time))
	{
		float rgba[4];
		GetColor(time,rgba);
		glColor4f(rgba[0],rgba[1],rgba[2],rgba[3]);
		switch(mState)
		{
		case 0: RenderDiamondStrip(mScale);	break;
		case 1: RenderHorizontalBars(mScale); break;
		case 2: RenderFlat(); break;
		}		
	}
}
void Fade::StartFadeIn(float time, int type, float duration)
{
	SetSrcColor(0,0,0,1);
	SetDstColor(0,0,0,0);
	StartFade(time,duration);
	mState = type;
	mScale = 0;
}
void Fade::StartFadeOut(float time, int type, float duration)
{
	SetSrcColor(0,0,0,0);
	SetDstColor(0,0,0,1);
	StartFade(time,duration);
	mState = type;
	mScale = 0;
}
//================================================================================================//
						/*********************************************
						** Render diamonds in a grid over the screen **	
						**********************************************/
//================================================================================================//
void Fade::RenderDiamondStrip(float scale)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(int x=0;x<704;x+=64)
	{
		for(int y=0;y<544;y+=64)
		{
			glVertex2f((float)x,(float)y-scale);
			glVertex2f((float)x+scale,(float)y);
			glVertex2f((float)x,(float)y+scale);
			glVertex2f((float)x-scale,(float)y);
		}
	}
	glEnd();
}
//================================================================================================//
						/********************************
						** Render bars over the screen **	
						*********************************/
//================================================================================================//
void Fade::RenderHorizontalBars(float scale)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(int y=0;y<480;y+=32)
	{
		glVertex2f((float)0,(float)y);
		glVertex2f((float)640,(float)y);
		glVertex2f((float)640,(float)y+scale);
		glVertex2f((float)0,(float)y+scale);
		scale*=0.95f;
	}
	glEnd();
}
//================================================================================================//
						/*******************************
						** standard screen sized quad **	
						********************************/
//================================================================================================//
void Fade::RenderFlat()
{
	glDisable(GL_TEXTURE_2D);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(640,0);
	glVertex2f(640,480);
	glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
}
