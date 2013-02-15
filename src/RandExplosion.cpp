#include "RandExplosion.h"
#include "Engine.h"

RandExplosion::RandExplosion()
{
}
//================================================================================================//
						/*******************
						** RandExplosion spawn **	
						********************/
//================================================================================================//
void RandExplosion::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	fLife = fStartLife = 5;//
	mSphere = Sphere(15,pos);
	iNextExplosion = 0;
	iNextLargeExplosion = 0;
}
//================================================================================================//
						/*******************
						** RandExplosion Clone **	
						********************/
//================================================================================================//
RandExplosion* RandExplosion::Clone()const
{
	return new RandExplosion(*this);
}
//================================================================================================//
						/********************
						** RandExplosion update **	
						*********************/
//================================================================================================//
void RandExplosion::Update()
{
	iNextExplosion++;
	iNextLargeExplosion++;
	fLife-=0.02f;
	if(fLife<=0)
		IsActive = false;
	if(iNextExplosion>=10)
	{
		Vec2 p = Pos,u,r;
		float rot = (float)(rand()%360);
		UTIL_Misc::MakeVectors(rot,u,r);
		p+=r*(float)(rand()%50);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,p,150,0.75f, (float)(rand()%360),false);
		SND_SMALLEXPLODE;
		iNextExplosion = 0;
	}
	if(iNextLargeExplosion>=25)
	{
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Pos,220,0.5f, (float)(rand()%360),true);
		SND_LARGEEXPLODE;
		iNextLargeExplosion = 0;
	}
}
//================================================================================================//
						/******************
						** RandExplosion draw **	
						*******************/
//================================================================================================//
void RandExplosion::Draw(const float interp)
{

}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void RandExplosion::NeedsToBeRemoved()
{
	if(Pos.x<gpEngine->Scroll)
		IsActive = false;

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool RandExplosion::CheckCollided(Sphere s, float damage)
{
	return false;
}

void RandExplosion::LoadFromFile(CFileIO &fIO)
{
}
void RandExplosion::WriteToFile(CFileIO &fIO)
{
}
int RandExplosion::InWater()
{
	return 0;
}