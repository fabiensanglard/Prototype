#include "PowerUp.h"
#include "Engine.h"

PowerUp::PowerUp()
{
	iType = 0;
}
//================================================================================================//
						/*******************
						** PowerUp spawn **	
						********************/
//================================================================================================//
void PowerUp::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 1;
	mSphere = Sphere(12,pos+Vec2(16,16));
	iTakeDamageTicks = 0;
}
//================================================================================================//
						/*******************
						** PowerUp Clone **	
						********************/
//================================================================================================//
PowerUp* PowerUp::Clone()const
{
	return new PowerUp(*this);
}
//================================================================================================//
						/********************
						** PowerUp update **	
						*********************/
//================================================================================================//
void PowerUp::Update()
{
	oPos = Pos;
	if(iType<4)
		frame<7? frame+=0.5f: frame-=7;
	else if(iType>=4)
		frame<7? frame+=0.25f: frame-=7;

	if(!gpEngine->mPlayer.bIsDead)
	if(Collision::SphereSphereOverlap(mSphere,Sphere(18,gpEngine->mPlayer.mSphere.p)))
	{
		SND_PICKUP;
		if(iType<4)
		{
			if(!gpEngine->mPlayer.bHasOrb)
			{
				gpEngine->mPlayer.bSpawnOrb = true;
				gpEngine->mPlayer.bHasOrb = true;
				gpEngine->mPlayer.mOrb.Spawn(gpEngine->mPlayer.Pos);
				gpEngine->mPlayer.mOrb.GiveTarget(&gpEngine->mPlayer.Pos);

				SND_ORBSPAWN;
			}
			gpEngine->mPlayer.mOrb.iWeaponType = iType;
		}
		else if(iType==4)
		{
			gpEngine->mPlayer.Speed+=0.15f;
			gpEngine->SpawnSpriteEffect(1,Vec2(-22,32),Vec2(96,96),gpEngine->sprSpeedBoost,0.75f,true,&gpEngine->mPlayer.Pos,true);
		}
		else if(iType==5)
		{
			gpEngine->mPlayer.iMissileType = 1;
		}
		else if(iType==6)
		{
			if(gpEngine->mPlayer.numBits<2)
				gpEngine->mPlayer.numBits++;
		}
		else if(iType==7)
		{
			gpEngine->mPlayer.iMissileType = 2;
		}
		gpEngine->mPlayer.iScore+=50;
		IsActive = false;
	}
}
//================================================================================================//
						/******************
						** PowerUp draw **	
						*******************/
//================================================================================================//
void PowerUp::Draw(const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	if(iType==4)
		RenderSprite(gpEngine->sprPowerUp,(int)frame+(8*iType),p.x,p.y,40,40);
	else
		RenderSprite(gpEngine->sprPowerUp,(int)frame+(8*iType),p.x,p.y,32,32);

	glDisable(GL_BLEND);
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void PowerUp::NeedsToBeRemoved()
{
	if(Pos.x+32<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool PowerUp::CheckCollided(Sphere s, float damage)
{
	return false;
}

void PowerUp::LoadFromFile(CFileIO &fIO)
{
}
void PowerUp::WriteToFile(CFileIO &fIO)
{
}
int PowerUp::InWater()
{
	return 0;
}