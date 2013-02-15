#include "SuperTurret.h"
#include "Engine.h"
#include "Geo2D.h"
#include "Debris.h"
SuperTurret::SuperTurret()
{
	pTarget = NULL;
	bDirection = true;
}
//================================================================================================//
						/*****************
						** Turret Spawn **	
						******************/
//================================================================================================//
void SuperTurret::Spawn(Vec2 pos)
{
	IsActive = true;
	frame = 0;
	oPos = Pos = pos;
	fWaitTime = gpEngine->mTimer.GetTime();
	fStartLife = fLife = 20;
	if(bDirection)
		mSphere = Sphere(16,pos+Vec2(32,20));
	else
		mSphere = Sphere(16,pos+Vec2(32,10));

	bWaitBeforeFire = false;
	iTakeDamageTicks=0;
}
//================================================================================================//
						/******************
						** Turret Update **	
						*******************/
//================================================================================================//
void SuperTurret::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;

	frame<15? frame+=0.5f: frame=15;
	if(frame==15 && !bWaitBeforeFire)
	{
		SND_LASER4;
		fWaitTime = gpEngine->mTimer.GetTime()+((float)2*gpEngine->fGameDifficulty);
		bWaitBeforeFire = true;

		//fire off 5 bullets
		Bullet b;
		b.pSpawn = Spawn_PowerBullet;
		b.pUpdate = Update_BasicBullet;
		b.pRender = Render_PowerBullet;
		b.pCollide = Collide_PowerBullet;
		(*b.pSpawn)(b,Pos+Vec2(16,0),Vec2(-1,0)*5);
		b.rot = 0;
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Pos+Vec2(30,0),Vec2(1,0)*5);
		b.rot = 180;
		gpEngine->SpawnBullet(b,false);
		if(!bDirection)
		{
			(*b.pSpawn)(b,Pos+Vec2(22,0),Vec2(0.5f,0.5f)*5);
			b.rot = -135;
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,Pos+Vec2(22,0),Vec2(-0.5f,0.5f)*5);
			b.rot = -45;
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,Pos+Vec2(22,8),Vec2(0,1)*5);
			b.rot = -90;
			gpEngine->SpawnBullet(b,false);
		}
		else
		{
			(*b.pSpawn)(b,Pos+Vec2(22,0),Vec2(0.5f,-0.5f)*5);
			b.rot = 135;
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,Pos+Vec2(22,0),Vec2(-0.5f,-0.5f)*5);
			b.rot = 45;
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,Pos+Vec2(22,-8),Vec2(0,-1)*5);
			b.rot = 90;
			gpEngine->SpawnBullet(b,false);
		}
	}
	else if(frame==15 && bWaitBeforeFire)
	{
		if(gpEngine->mTimer.GetTime()>fWaitTime)
		{
			frame=0;
			bWaitBeforeFire = false;
		}
	}
}
//================================================================================================//
						/*****************
						** Turret Clone **	
						******************/
//================================================================================================//
SuperTurret* SuperTurret::Clone()const
{
	return new SuperTurret(*this);
}
//================================================================================================//
						/****************
						** Turret Draw **	
						*****************/
//================================================================================================//
void SuperTurret::Draw(const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	if(bDirection)
		RenderSprite(gpEngine->sprSuperTurret,(int)frame,Pos.x,Pos.y,64,32);
	else
		RenderSprite(gpEngine->sprSuperTurret,(int)frame,Pos.x,Pos.y+32,64,-32);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 5);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void SuperTurret::NeedsToBeRemoved()
{
	if(Pos.x+64<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool SuperTurret::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		if(bDirection)
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,20),110,0.75f,(float)(rand()%360),false);
		else
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,10),110,0.75f,(float)(rand()%360),false);
		SND_LARGEEXPLODE;
		gpEngine->mPlayer.iScore+=100;

		float debRot = 0;
		float incRot = 360.0f/10.0f;
		Vec2 debUp,debRight;
		for(int n=0;n<10;n++)
		{
			Debris* deb = new Debris;
			UTIL_Misc::MakeVectors(debRot,debUp,debRight);
			deb->Spawn(mSphere.p);
			deb->Vel = debRight * ((float)(rand()%4)+1);
			gpEngine->GiveEntityToList(deb);
			debRot+=incRot;
		}
	}
	iTakeDamageTicks = 3;
	return true;
}

void SuperTurret::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bDirection,1);
}
void SuperTurret::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bDirection,1);
}
int SuperTurret::InWater()
{
	return 0;
}