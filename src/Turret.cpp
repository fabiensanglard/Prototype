#include "Turret.h"
#include "Engine.h"
#include "Geo2D.h"
#include "Debris.h"
Turret::Turret()
{
	pTarget = NULL;
	RateofFire = 5;
	bDirection = true;
}
//================================================================================================//
						/*****************
						** Turret Spawn **	
						******************/
//================================================================================================//
void Turret::Spawn(Vec2 pos)
{
	IsActive = true;
	frame = 4;
	oPos = Pos = pos;
	fLastFireTime = gpEngine->mTimer.GetTime();
	fStartLife = fLife = 3;
	if(bDirection)
		mSphere = Sphere(12,pos+Vec2(24,20));
	else
		mSphere = Sphere(12,pos+Vec2(24,10));

	iTakeDamageTicks=0;
}
//================================================================================================//
						/******************
						** Turret Update **	
						*******************/
//================================================================================================//
void Turret::Update()
{
	iTakeDamageTicks--;

	if(!pTarget)
		return;

	Vec2 p = *pTarget + Vec2(32,32);
	Vec2 p2(Pos.x+32,Pos.y+32);

	Line l1(p,p2);
	Line l2(p2,Vec2(p2.x,p2.y+100));
	float angle = RAD2DEG(l1.Angle(l2));

	bDirection?
		angle = UTIL_Misc::Clamp(angle,0,90):
		angle = 90-(UTIL_Misc::Clamp(angle,90,180)-90);
	int desiredFrame = 4;
	if(p.x<Pos.x)
		desiredFrame-=(int)(angle/22);
	else
		desiredFrame+=(int)(angle/22);

	if(frame<(float)desiredFrame)
		frame+=0.3f;
	if(frame>(float)desiredFrame)
		frame-=0.3f;

	if(RateofFire>0)
	if(gpEngine->mTimer.GetTime()>=fLastFireTime+((float)RateofFire*gpEngine->fGameDifficulty))
	{
		fLastFireTime = gpEngine->mTimer.GetTime();
		Bullet b;
		b.pSpawn = Spawn_BasicBullet;
		b.pUpdate = Update_BasicBullet;
		b.pRender = Render_BasicBullet;
		b.pCollide = Collide_BasicBullet;
		Vec2 v = FireAtTarget(Pos,*pTarget+Vec2(32,32),1.5f);
		(*b.pSpawn)(b,Pos,v);
		gpEngine->SpawnBullet(b,false);
		SND_LASER2;
	}
}
//================================================================================================//
						/*****************
						** Turret Clone **	
						******************/
//================================================================================================//
Turret* Turret::Clone()const
{
	return new Turret(*this);
}
//================================================================================================//
						/****************
						** Turret Draw **	
						*****************/
//================================================================================================//
void Turret::Draw(const float interp)
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
		RenderSprite(gpEngine->sprTurret,(int)frame,Pos.x,Pos.y-7,48,48);
	else
		RenderSprite(gpEngine->sprTurret,(int)frame,Pos.x,Pos.y+48-9,48,-48);
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
void Turret::NeedsToBeRemoved()
{
	if(Pos.x+48<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Turret::CheckCollided(Sphere s, float damage)
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
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,20),90,0.75f,(float)(rand()%360),false);
		else
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,10),90,0.75f,(float)(rand()%360),false);
		SND_MEDEXPLODE;
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

void Turret::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bDirection,1);
}
void Turret::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bDirection,1);
}
int Turret::InWater()
{
	return 0;
}