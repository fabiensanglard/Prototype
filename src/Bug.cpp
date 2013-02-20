#include "Bug.h"
#include "Engine.h"
#include "debris.h"

Bug::Bug()
{
	fRotation = 0;
	RateofFire = 0;
}
//================================================================================================//
						/*******************
						** Bug spawn **	
						********************/
//================================================================================================//
void Bug::Spawn(Vec2 pos)
{
	IsActive = true;
	fLastFireTime = gpEngine->mTimer.GetTime();
	bStartDirection? fRotation = -90: fRotation = -270;
	bStartDirection? pos.y = 512: pos.y=-32;
	oPos = Pos = pos;
	fLife = fStartLife = 2;
	mSphere = Sphere(14,Pos);
	iTakeDamageTicks = 0;
	frame = 0;
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
	mWaterticks = 0;
}

//================================================================================================//
						/*******************
						** Bug Clone **	
						********************/
//================================================================================================//
Bug* Bug::Clone()const
{
	return new Bug(*this);
}
//================================================================================================//
						/********************
						** Bug update **	
						*********************/
//================================================================================================//
void Bug::Update()
{
	iTakeDamageTicks--;
	if(!gpEngine->bRecording || (gpEngine->bRecording && gpEngine->pRecordEnt != this))
		PlayBack();

	oPos = Pos;
	InWater();

	if(KEY_RIGHT.state == PRESSED)
	{
		fRotation += 1.85f;
	}
	if(KEY_LEFT.state == PRESSED)
	{
		fRotation -= 1.85f;
	}

	Vec2 u,r;
	UTIL_Misc::MakeVectors(fRotation,u,r);
	Pos+=u*2.5f;

	fRotation = UTIL_Misc::Wrap(fRotation,360);
	frame<15? frame+=0.75f: frame=0;

	if(RateofFire>0)
	if(gpEngine->mTimer.GetTime()>=fLastFireTime+((float)RateofFire*gpEngine->fGameDifficulty))
	{
		fLastFireTime = gpEngine->mTimer.GetTime();
		Bullet b;
		b.pSpawn = Spawn_BasicBullet;
		b.pUpdate = Update_BasicBullet;
		b.pRender = Render_BasicBullet;
		b.pCollide = Collide_BasicBullet;
		Vec2 v;
		if(FireAtDistantTarget(mSphere.p,gpEngine->mPlayer.mSphere.p,3.5f,v))
		{
			(*b.pSpawn)(b,mSphere.p,v);
			gpEngine->SpawnBullet(b,false);
			SND_LASER2;
		}
	}
}
//================================================================================================//
						/******************
						** Bug draw **	
						*******************/
//================================================================================================//
void Bug::Draw(const float interp) 
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	RenderRotatedSprite(gpEngine->sprBug,(int)frame,p.x,p.y,22,22,fRotation);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/******************************
						** if its buffer has ran out **	
						*******************************/
//================================================================================================//
void Bug::NeedsToBeRemoved()
{
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;

	if(Pos.x+48<(gpEngine->Scroll-100))
		IsActive = false;
	if(mKeyBuffer.empty())
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Bug::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos,100,0.75f,(float)(rand()%360),false);
		gpEngine->smpSmallExplode.Play(3);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=25;

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

void Bug::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartDirection,1);
	ReadBufferFromFile(fIO);
}
void Bug::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartDirection,1);
	WriteBufferToFile(fIO);
}
int Bug::InWater()
{
	mWaterticks++;
	if(mWaterticks<5)
		return 0;
	mWaterticks=0;
	int x = (int)(mSphere.p.x/32);
	int y = (int)(mSphere.p.y/32);
	if(gpEngine->mTilesLayer1[x][y].ID==1018)//water surface
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);

	return 0;
}