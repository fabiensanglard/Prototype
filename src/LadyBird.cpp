#include "LadyBird.h"
#include "Engine.h"
#include "Debris.h"
LadyBird::LadyBird()
{
	sinus = 180;
	RateofFire = 0;
}
//================================================================================================//
						/*******************
						** ladybird spawn **	
						********************/
//================================================================================================//
void LadyBird::Spawn(Vec2 pos)
{
	IsActive = true;
	pos.x = gpEngine->Scroll+640;
	oPos = Pos = pos;
	frame = 0;
	fLastFireTime = gpEngine->mTimer.GetTime();
	fStartLife = fLife = 1;
	mSphere = Sphere(15,pos+Vec2(24,24));
	iTakeDamageTicks = 0;
	sinus = 0;
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** ladybird Clone **	
						********************/
//================================================================================================//
LadyBird* LadyBird::Clone()const
{
	return new LadyBird(*this);
}
//================================================================================================//
						/********************
						** ladybird update **	
						*********************/
//================================================================================================//
void LadyBird::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	sinus+=0.025f;
	Pos.x-=1.5f;
	bStartMovement?
		Pos.y-=sin(sinus):
		Pos.y+=sin(sinus);
	frame<15? frame+=0.25f: frame-=15;

	InWater();

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
						** ladybird draw **	
						*******************/
//================================================================================================//
void LadyBird::Draw(const float interp)
{
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);

	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(gpEngine->sprLadyBird,(int)frame,p.x+48,p.y,-48,48);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 5);*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void LadyBird::NeedsToBeRemoved()
{
	if(Pos.x+48<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool LadyBird::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,24),90,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Pos+Vec2(24,24),120,0.5f, (float)(RotAngle),true);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=15;

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

void LadyBird::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartMovement,1);
}
void LadyBird::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartMovement,1);
}
int LadyBird::InWater()
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