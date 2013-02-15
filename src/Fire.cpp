#include "Fire.h"
#include "Engine.h"
//================================================================================================//
						/********************************************************
						** Doesnt really do anything, more like a dummy entity **	
						*********************************************************/
//================================================================================================//

Fire::Fire()
{
}
void Fire::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
}
Fire* Fire::Clone()const
{
	return new Fire(*this);
}
void Fire::Update()
{
}
void Fire::Draw(const float interp)
{
	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgSplashFire);
	for(int f=0;f<1;f++)
	{
		float x = Pos.x;
		float y = Pos.y;
		float sinus=0.3f;
		float fire;
		float t=gpEngine->mTimer.GetTime();
		if(f>0)
			fire = sin(t*=1.5f)*10;
		else
			fire = sin(t*5)*10;
		glBegin(GL_QUADS);
		if(!bTop)
		{
			glTexCoord2f((float)(f?1:0),0); glVertex2f(x+fire,					y);
			glTexCoord2f((float)(f?0:1),0); glVertex2f(x+512+fire,				y);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(x+512+fire*0.75f,	y+32);
			glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(x+fire*0.75f,		y+32);
			glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(x+fire*0.75f,		y+32);
			glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(x+512+fire*0.75f,	y+32);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(x+512+fire*0.5f,		y+64);
			glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(x+fire*0.5f,			y+64);
			glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(x+fire*0.5f,			y+64);
			glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(x+512+fire*0.5f,		y+64);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(x+512+fire*0.25f,	y+96);
			glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(x+fire*0.25f,		y+96);
			glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(x+fire*0.25f,		y+96);
			glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(x+512+fire*0.25f,	y+96);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),1); glVertex2f(x+512,					y+128);
			glTexCoord2f((float)(f?1:0),1); glVertex2f(x,						y+128);
			glEnd();
		}
		else
		{
			y=y+96;
			glTexCoord2f((float)(f?1:0),0); glVertex2f(x+fire,					y);
			glTexCoord2f((float)(f?0:1),0); glVertex2f(x+512+fire,				y);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(x+512+fire*0.75f,	y-32);
			glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(x+fire*0.75f,		y-32);
			glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(x+fire*0.75f,		y-32);
			glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(x+512+fire*0.75f,	y-32);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(x+512+fire*0.5f,		y-64);
			glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(x+fire*0.5f,			y-64);
			glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(x+fire*0.5f,			y-64);
			glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(x+512+fire*0.5f,		y-64);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(x+512+fire*0.25f,	y-96);
			glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(x+fire*0.25f,		y-96);
			glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(x+fire*0.25f,		y-96);
			glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(x+512+fire*0.25f,	y-96);
			fire = sin((t+=sinus)*5)*10;
			glTexCoord2f((float)(f?0:1),1); glVertex2f(x+512,					y-128);
			glTexCoord2f((float)(f?1:0),1); glVertex2f(x,						y-128);
			glEnd();
		}
	
	}
	glDisable(GL_BLEND);
}
void Fire::NeedsToBeRemoved()
{
	if(Pos.x+512<=gpEngine->Scroll)
		IsActive = false;
}
bool Fire::CheckCollided(Sphere s, float damage)
{
	return false;
}
void Fire::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bTop,1);
}
void Fire::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bTop,1);
}
int Fire::InWater()
{
	return 0;
}