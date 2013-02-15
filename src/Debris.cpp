#include "Debris.h"
#include "Engine.h"

Debris::Debris()
{
}
//================================================================================================//
						/*******************
						** Debris spawn **	
						********************/
//================================================================================================//
void Debris::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	fLife = fStartLife = (float)(rand()%4)+1;
	frame = (float)(rand()%8);
	fRotation = (float)(rand()%10)+1;
	fRot = (float)(rand()%360);
	rand()%2? bRotation = true: bRotation = false;
	mSphere = Sphere(15,pos+Vec2(16,16));
	iTakeDamageTicks = 0;
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** Debris Clone **	
						********************/
//================================================================================================//
Debris* Debris::Clone()const
{
	return new Debris(*this);
}
//================================================================================================//
						/********************
						** Debris update **	
						*********************/
//================================================================================================//
void Debris::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;

	InWater();

	bRotation?
		fRot += fRotation:
		fRot -= fRotation;

	fLife-=0.1f;
	if(fLife<=0)
		IsActive = false;
	Vel*=0.992f;
	Pos+=Vel;
}
//================================================================================================//
						/******************
						** Debris draw **	
						*******************/
//================================================================================================//
void Debris::Draw(const float interp)
{
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
	{
		float a = fLife;
		if(a>1)a=1;
		if(a<0)a=0;
		glColor4f(1,1,1,a);
	}
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);

	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderRotatedSprite(gpEngine->sprTrash,(int)frame,p.x+16,p.y+16,4,4,fRot);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 10);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void Debris::NeedsToBeRemoved()
{
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Debris::CheckCollided(Sphere s, float damage)
{
	return false;
}

void Debris::LoadFromFile(CFileIO &fIO)
{
}
void Debris::WriteToFile(CFileIO &fIO)
{
}
int Debris::InWater()
{
	return 0;
}