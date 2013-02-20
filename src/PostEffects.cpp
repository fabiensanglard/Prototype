#include "engine.h"
//================================================================================================//
				/*************************************************************************
				** if we have pbuffer support, we'll copy the main frame into one image **	
				**************************************************************************/
//================================================================================================//
void Engine::BindMainContext()
{
	if(!bRenderTargetSupport)
	{
		UTIL_GL::GL2D::SetOrtho(640,480);
		return;
	}
	glEnable(GL_TEXTURE_2D);
//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Lock_pBuffer(pMainTarget);
//#endif
	UTIL_GL::GL2D::SetOrtho(640,480);

}
//================================================================================================//
							/***********************
							** Finalize the frame **	
							************************/
//================================================================================================//
void Engine::FinalizeMainContext()
{
	if(!bRenderTargetSupport)
		return;

	glEnable(GL_TEXTURE_2D);
//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Unlock_pBuffer(pMainTarget);
//#endif
	RenderMainContext();
}
//================================================================================================//
							/****************************************
							** render the context plain and simple **	
							*****************************************/
//================================================================================================//
void Engine::RenderMainContext()
{
	glEnable(GL_TEXTURE_2D);
//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Bind_pBuffer(*pMainTarget);
	
//#endif
//	UTIL_GL::GL2D::SetOrtho(640,480);
	glColor4f(1,1,1,1);
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2f(0,0);
		glTexCoord2f(1,1);
		glVertex2f(640,0);
		glTexCoord2f(1,0);
		glVertex2f(640,480);
		glTexCoord2f(0,0);
		glVertex2f(0,480);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//================================================================================================//
							/********************************
							** Render post procces effects **	
							********************************/
//================================================================================================//
void Engine::DoPostEffects(float interp)
{

	PostWater(interp);
}
//================================================================================================//
							/***********************************
							** Render weapon charge up effect **	
							***********************************/
//================================================================================================//
#define BUFMAPX(x)	x/1024.0f
#define BUFMAPY(y)	y/512.0f
static float gCharge =180;
void Engine::PostCharge(float interp)
{
	if(!bRenderTargetSupport)
		return;

//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Unlock_pBuffer(pMainTarget);
	glEnable(GL_TEXTURE_2D);
	UTIL_SDL::Lock_pBuffer(p64x64Target);
//#endif
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,64,64);
	UTIL_GL::GL2D::SetOrtho(64,64);
	float scr = UTIL_Misc::Interpolate(Scroll,oScroll,interp);
	float px=UTIL_Misc::Interpolate(mPlayer.Pos.x,mPlayer.oPos.x,interp)-scr;
	float py=UTIL_Misc::Interpolate(mPlayer.Pos.y,mPlayer.oPos.y,interp);
	glTranslatef(-(px+64),-py,0);
	RenderMainContext();
	glTranslatef(px+64,py,0);

	px+=64;
	glEnable(GL_TEXTURE_2D);
//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Unlock_pBuffer(p64x64Target);
	BindMainContext();
	UTIL_SDL::Bind_pBuffer(*p64x64Target);
//#endif
	glDisable(GL_BLEND);
	float hpx = (1.0f/64.0f)*0.5f;
	float hpy = (1.0f/64.0f)*0.5f;
	float time = (mTimer.GetTime() - fForceChargeStart);
	if(time>1.5f)time=1.5f;
	float time2=time/1.5f;
	time/=6.0f;
	float s = sin(mTimer.GetTime()*5)*0.1f;
	float t = cos(mTimer.GetTime()*5)*0.1f;
	s*=time2;
	t*=time2;
	if(s<0)s=0;
	if(s>1)s=1;
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1,1,1,1);
	glTexCoord2f((0.5f+time)+s,0.5f+t);
	glVertex2f(px+32,py+32);
	glTexCoord2f(0+hpx,1-hpy);
	glVertex2f(px,py);
	glTexCoord2f(1-hpx,1-hpy);
	glVertex2f(px+64,py);
	glTexCoord2f(1-hpx,0+hpy);
	glVertex2f(px+64,py+64);
	glTexCoord2f(0+hpx,0+hpy);
	glVertex2f(px,py+64);
	glTexCoord2f(0+hpx,1-hpy);
	glVertex2f(px,py);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//================================================================================================//
							/*****************************
							** render underwater effect **	
							******************************/
//================================================================================================//
void Engine::PostWater(float interp)
{
	if(!bRenderTargetSupport)
		return;

	Tile *pTiles;
	pTiles = *mTilesLayer1;

	float scr = UTIL_Misc::Interpolate(Scroll,oScroll,interp);
	int x1=-1,x2,y1=-1,y2;
	int sx=(int)(scr/32);
	int x,y;
	for( x=sx; x<(sx+21); x++)
	{
		for( y=0; y<15; y++)
		{
			if(x>399 || x<0)
				continue;
			if(pTiles[y+(x*15)].ID<0)
				continue;
			if(pTiles[y+(x*15)].ID>=1019)
			{
				if(x1<0)x1=x;
				x2=x;
				if(y1<0)y1=y;
				y2=y;

			}
		}
	}
	if(x1<0 || y1<0)
		return;
	x1*=32;
	x2*=32;
	y1*=32;
	y2*=32;
	x1-=(int)scr;
	x2-=(int)scr;

	if(x1<0)x1=0;
	x2=(x2-x1)+32;
	if(x2>640)x2=640;
//#ifdef SDL_VERSION_1_3
	UTIL_SDL::Bind_pBuffer(*pMainTarget);
//#endif
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1.0f,1,1);

	glEnable(GL_SCISSOR_TEST);
	glScissor(x1,480-(y2+32),x2,(y2-y1)+32);

	
	float th = (1.0f/16.0f);
	gCharge+=0.1f;
	float rs = sin(gCharge)*0.005f;
	float rt = sin(gCharge*0.5f)*0.005f;

	int sway=0;
	glBegin(GL_QUADS);
	for(y=0;y<512;y+=32)
	{
		float s = 0;
		float s2 = 1;
		float s3 = 1;
		float s4 = 0;
		switch(sway)
		{
		case 0:
			s+=rs; s2+=rs; s3-=rs; s4-=rs;
			break;
		case 1:
			s-=rs; s2-=rs; s3-=rt; s4-=rt;
			break;
		case 2:
			s-=rt; s2-=rt; s3+=rt; s4+=rt;
			break;
		case 3:
			s+=rt; s2+=rt; s3+=rs; s4+=rs;
			break;
		}
	
		if(s<0)
			s=0;
		if(s2>1)
			s2=1;
		if(s3>1)
			s3=1;
		if(s4<0)
			s4=0;
		float t=1.0f-(th*(y/32));
		glTexCoord2f(s,t);
		glVertex2f(0,(float)y);
		glTexCoord2f(s2,t);
		glVertex2f(1024,(float)y);
		glTexCoord2f(s3,t-th);
		glVertex2f(1024,(float)y+32);
		glTexCoord2f(s4,t-th);
		glVertex2f(0,(float)y+32);

		sway<3? sway++:sway=0;
	}
	glEnd();
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}