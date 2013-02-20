#include "Engine.h"
#include "UTIL_bitop.h"
//================================================================================================//
						/*********************************
						** Little cleanup of the arrays **	
						**********************************/
//================================================================================================//
void Engine::ClearMapArrays()
{
	for(int x=0;x<400;x++)
	{
		for(int y=0;y<15;y++)
		{
			mTilesLayer1[x][y].ID = -1;
			mTilesLayer1[x][y].properties = 0;
			mTilesLayer2[x][y].ID = -1;
			mTilesLayer2[x][y].properties = 0;
			mTilesLayer3[x][y].ID = -1;
			mTilesLayer3[x][y].properties = 0;
		}
	}
}
//================================================================================================//
						/****************************************
						** Render all visible tiles in a layer **	
						****************************************/
//================================================================================================//
void Engine::RenderMapLayer(int layer, float interp, bool editing)
{
	Tile *pTiles;
	switch(layer)
	{
	case 1: pTiles = *mTilesLayer1; break;
	case 2: pTiles = *mTilesLayer2; break;
	case 3: pTiles = *mTilesLayer3; break;
	}
	pair<float,float>st;
	pair<float,float>scale;
	float s,t,s2,t2;
	float tw;
	float th;

	scale = sprMapTiles.GetTextureScale();
	tw = scale.first;
	th = scale.second;
	float railMulti=1;
	if(layer>1 && mPlayer.bHasOrb && mPlayer.mOrb.iWeaponType == 1)
	{
			if(mPlayer.iForceCharge>100)
			{
				float tick = (float)(mPlayer.iForceCharge-100);
				if(tick>50)tick=50;
				railMulti=1.0f - (tick/65.0f);
			}
	}
	glColor4f((1.0f/(float)layer)*railMulti,(1.0f/(float)layer)*railMulti,(1.0f/(float)layer)*railMulti,1);
//	glColor4f(1.0f,1.0f,1.0f,1);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,sprMapTiles.imgID);
	
	float scr = UTIL_Misc::Interpolate(Scroll,oScroll,interp);
	if(!editing)
	{
		if(layer==2)
			scr *= 0.75f;
		else if(layer==3)
			scr *= 0.5f;
	}

	int sx=(int)(scr/32);
	float fx,fy;
	float halfPixel=((1.0f/1024)*0.5f);
	for(int x=sx; x<(sx+21); x++)
	{
		fx=(float)x;
		for(int y=0; y<15; y++)
		{
			fy = (float)y;
			if(x>399 || x<0)
				continue;
			if(pTiles[y+(x*15)].ID<0)
				continue;
			if(pTiles[y+(x*15)].ID>=1017 && !bUseEditor)//water
				continue;

			if(pTiles[y+(x*15)].ID>=1011 && pTiles[y+(x*15)].ID<1017)//stars
				UTIL_GL::SetBlend(GL_ONE,GL_ONE);
			else
				UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			st = sprMapTiles.GetFrameOffsets(pTiles[y+(x*15)].ID);
			s = s2 = st.first;
			t = t2 = st.second;
			if(UTIL_IsBitSet<unsigned char>(pTiles[y+(x*15)].properties,FLIP_HORIZONTAL))
			{
				s+=tw-(pTiles[y+((x+1)*15)].ID>=0? 0:halfPixel);
				s2+=(pTiles[y+((x-1)*15)].ID>=0? 0:halfPixel);
			}
			else
			{
				s+=(pTiles[y+((x-1)*15)].ID>=0? 0:halfPixel);
				s2+=tw-(pTiles[y+((x+1)*15)].ID>=0? 0:halfPixel);
			}
			if(UTIL_IsBitSet<unsigned char>(pTiles[y+(x*15)].properties,FLIP_VERTICAL))
			{
				t+=th-(pTiles[(y+1)+(x*15)].ID>=0? 0:halfPixel);
				t2+=(pTiles[(y-1)+(x*15)].ID>=0? 0:halfPixel);
			}
			else
			{
				t+=(pTiles[(y-1)+(x*15)].ID>=0? 0:halfPixel);
				t2+=th-(pTiles[(y+1)+(x*15)].ID>=0? 0:halfPixel);
			}
			glBegin(GL_QUADS);
			glTexCoord2f(s,t);
			glVertex2f((fx*32),(fy*32));
			glTexCoord2f(s2,t);
			glVertex2f((fx*32)+32,(fy*32));
			glTexCoord2f(s2,t2);
			glVertex2f((fx*32)+32,(fy*32)+32);
			glTexCoord2f(s,t2);
			glVertex2f((fx*32),(fy*32)+32);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
//================================================================================================//
						/*******************
						** Update the map **	
						********************/
//================================================================================================//
static float wO =0;//water offset
void Engine::UpdateMap()
{
	oScroll = Scroll;
	if(!bPauseScrolling)
		Scroll += SCROLL_SPEED + fMapSpeed;
	if(Scroll>(380*32))
		Scroll = 380*32;

	wO+=0.01f;
}
//================================================================================================//
						/******************************************
						** render transparent water (final pass) **	
						*******************************************/
//================================================================================================//

void Engine::RenderWaterTransparent(const float interp)
{
	Tile *pTiles;
	pTiles = *mTilesLayer1;

	pair<float,float>st;
	pair<float,float>scale;
	float s,t,s2,t2;
	float tw;
	float th;


	scale = sprMapTiles.GetTextureScale();
	tw = scale.first;
	th = scale.second;
	
	float scr = UTIL_Misc::Interpolate(Scroll,oScroll,interp);

	int sx=(int)(scr/32);
	float fx,fy;
	float halfPixel=((1.0f/1024)*0.5f);
	for(int y=0; y<15; y++)
	{
		fy = (float)y;
		for(int x=sx; x<(sx+21); x++)
		{
			fx=(float)x;
			if(x>399 || x<0)
				continue;
			if(pTiles[y+(x*15)].ID<0)
				continue;
			if(pTiles[y+(x*15)].ID<1018)//water
				continue;

			if(pTiles[y+(x*15)].ID>1018)//water
			{
				glColor4f(1,1,1,0.3f);
				UTIL_GL::BindTexture(GL_TEXTURE_2D,sprMapTiles.imgID);
				UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

				st = sprMapTiles.GetFrameOffsets(pTiles[y+(x*15)].ID);
				s = s2 = st.first;
				t = t2 = st.second;
				if(UTIL_IsBitSet<unsigned char>(pTiles[y+(x*15)].properties,FLIP_HORIZONTAL))
				{
					s+=tw-(pTiles[y+((x+1)*15)].ID>=0? 0:halfPixel);
					s2+=(pTiles[y+((x-1)*15)].ID>=0? 0:halfPixel);
				}
				else
				{
					s+=(pTiles[y+((x-1)*15)].ID>=0? 0:halfPixel);
					s2+=tw-(pTiles[y+((x+1)*15)].ID>=0? 0:halfPixel);
				}
				if(UTIL_IsBitSet<unsigned char>(pTiles[y+(x*15)].properties,FLIP_VERTICAL))
				{
					t+=th-(pTiles[(y+1)+(x*15)].ID>=0? 0:halfPixel);
					t2+=(pTiles[(y-1)+(x*15)].ID>=0? 0:halfPixel);
				}
				else
				{
					t+=(pTiles[(y-1)+(x*15)].ID>=0? 0:halfPixel);
					t2+=th-(pTiles[(y+1)+(x*15)].ID>=0? 0:halfPixel);
				}
				glBegin(GL_QUADS);
				glTexCoord2f(s,t);
				glVertex2f((fx*32),(fy*32));
				glTexCoord2f(s2,t);
				glVertex2f((fx*32)+32,(fy*32));
				glTexCoord2f(s2,t2);
				glVertex2f((fx*32)+32,(fy*32)+32);
				glTexCoord2f(s,t2);
				glVertex2f((fx*32),(fy*32)+32);
				glEnd();
			}
			else//surface
			{
				glColor4f(1,1,1,0.75f);
				UTIL_GL::BindTexture(GL_TEXTURE_2D,sprWater.imgID);
				UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
				glBegin(GL_QUADS);
				glTexCoord2f(0+wO,0);
				glVertex2f((fx*32),(fy*32)+4);
				glTexCoord2f(1+wO,0);
				glVertex2f((fx*32)+32,(fy*32)+4);
				glTexCoord2f(1+wO,1);
				glVertex2f((fx*32)+32,(fy*32)+32+4);
				glTexCoord2f(0+wO,1);
				glVertex2f((fx*32),(fy*32)+32+4);

				float wO2 = wO*2;
				glTexCoord2f(0-wO2,0);
				glVertex2f((fx*32),(fy*32)+4);
				glTexCoord2f(1-wO2,0);
				glVertex2f((fx*32)+32,(fy*32)+4);
				glTexCoord2f(1-wO2,1);
				glVertex2f((fx*32)+32,(fy*32)+32+4);
				glTexCoord2f(0-wO2,1);
				glVertex2f((fx*32),(fy*32)+32+4);
				glEnd();
			}
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
