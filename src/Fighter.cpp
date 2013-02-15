#include "Fighter.h"
#include "Engine.h"
#include "Debris.h"
Fighter::Fighter()
{
	RateofFire = 0;
}
//================================================================================================//
						/*******************
						** Fighter spawn **	
						********************/
//================================================================================================//
void Fighter::Spawn(Vec2 pos)
{
	IsActive = true;
	fLastFireTime = gpEngine->mTimer.GetTime();
	bStartDirection? 	pos.x = gpEngine->Scroll+640 : pos.x = gpEngine->Scroll-64;
	bStartDirection? bDirection = false: bDirection = true;
	oPos = Pos = pos;
	fLife = fStartLife = 2;
	mSphere = Sphere(14,Pos+Vec2(30,30));
	iTakeDamageTicks = 0;
	frame = 0;
	bTurning = false;
	bAnim = false;
	bCrashing = false;
	iNextSmokeTick = 0;
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
}

//================================================================================================//
						/*******************
						** Fighter Clone **	
						********************/
//================================================================================================//
Fighter* Fighter::Clone()const
{
	return new Fighter(*this);
}
//================================================================================================//
						/********************
						** Fighter update **	
						*********************/
//================================================================================================//
void Fighter::Update()
{
	iTakeDamageTicks--;
	if(!gpEngine->bRecording || gpEngine->bRecording && gpEngine->pRecordEnt != this)
		if(!bCrashing)
			PlayBack();

	oPos = Pos;

	if(!bCrashing)
	{
	if(!bTurning)
		if(KEY_RIGHT.state == PRESSED
			|| KEY_LEFT.state == PRESSED)
		{
			bTurning = true;
			frame = 0;
		}
	if(KEY_UP.state == PRESSED)
		Vel.y-=0.15f;
	if(KEY_DOWN.state == PRESSED)
		Vel.y+=0.15f;
	}
	else
		Vel.y+=1;

	Vel.y*=0.89f;
	Pos.y+=Vel.y;

	if(bCrashing)
	{
		if(CollidedWithMap())
		{
			IsActive = false;
			int RotAngle = rand()%360;
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,130,0.75f, (float)(RotAngle),false);
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,170,0.5f, (float)(RotAngle),true);
			SND_MEDEXPLODE;
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
		bDirection? Pos.x+=2.5f: Pos.x-=2.5f;
		iNextSmokeTick++;
		if(iNextSmokeTick>10)
		{
			iNextSmokeTick = 0;
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,70,0.85f, (float)(rand()%360),true);
		}
		switch(iNextSmokeTick)
		{
		case 0: case 3: case 6: case 9:
			gpEngine->SpawnExplosion(gpEngine->sprSmokePuff,mSphere.p,50,0.5f, (float)(rand()%360),true);
			break;
		}
		return;
	}

	if(bTurning)
	{
		frame<15? frame+=0.25f: frame=15;
		float f=(frame-7.5f)/7.5f;
		bDirection? Pos.x-=2.5f*f: Pos.x+=2.5f*f;
		if(frame==15)
		{
			bTurning = false;
			bDirection = !bDirection;
		}
	}
	else
	{
		if(!bAnim)
			frame<15? frame+=0.75f: (frame=15, bAnim =true);
		else
			frame>0? frame-=0.75f: (frame=0, bAnim = false);
		bDirection? Pos.x+=2.5f: Pos.x-=2.5f;
	}
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
						** Fighter draw **	
						*******************/
//================================================================================================//
void Fighter::Draw(const float interp) 
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
	int f = (int)frame;
	if(bTurning)
		f+=16;
	if(bDirection)
		RenderSprite(gpEngine->sprFighter,f,p.x,p.y,48,48);
	else
		RenderSprite(gpEngine->sprFighter,f,p.x+48,p.y,-48,48);

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
void Fighter::NeedsToBeRemoved()
{
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;

	if(Pos.x+48<(gpEngine->Scroll-100))
		IsActive = false;
	if(mKeyBuffer.empty())
		IsActive = false;

	if(bCrashing)
	{
		if(Pos.y>500)
			IsActive = false;
	}
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Fighter::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	if(bCrashing)
	{
		IsActive = false;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,130,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,170,0.5f, (float)(RotAngle),true);
		SND_MEDEXPLODE;
		gpEngine->mPlayer.iScore+=10;

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
		return true;
	}
	fLife -= damage;
	if(fLife<=0)
	{
		bCrashing = true;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,100,0.75f,(float)(rand()%360),false);
		gpEngine->smpSmallExplode.Play(3);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=25;
	}
	iTakeDamageTicks = 3;
	return true;
}

void Fighter::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartDirection,1);
	ReadBufferFromFile(fIO);
}
void Fighter::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartDirection,1);
	WriteBufferToFile(fIO);
}
int Fighter::InWater()
{
	return 0;
}