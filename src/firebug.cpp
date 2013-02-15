#include "FireBug.h"
#include "Engine.h"
#include "Debris.h"
FireBug::FireBug()
{
	RateofFire = 0;
}
//================================================================================================//
						/*******************
						** FireBug spawn **	
						********************/
//================================================================================================//
void FireBug::Spawn(Vec2 pos)
{
	IsActive = true;
	bStartMovement?
		pos.x = gpEngine->Scroll+640:
		pos.x = gpEngine->Scroll-64;
		
	oPos = Pos = pos;
	Vel = Vec2(0,0);
	frame = 0;
	fLastFireTime = gpEngine->mTimer.GetTime();
	fStartLife = fLife = 1;
	mSphere = Sphere(15,pos+Vec2(14,24));
	iTakeDamageTicks = 0;
	rot = 0;
	mWaterticks = 0;
	bDive = false;
	Pos.y>240?
		bDiveDirection = true://up
		bDiveDirection = false;//down

}
//================================================================================================//
						/*******************
						** FireBug Clone **	
						********************/
//================================================================================================//
FireBug* FireBug::Clone()const
{
	return new FireBug(*this);
}
//================================================================================================//
						/********************
						** FireBug update **	
						*********************/
//================================================================================================//
void FireBug::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	bStartMovement?
		Pos.x-=2.4f:
		Pos.x+=2.4f;

	Pos.y+=Vel.y;
	bDiveDirection?
		rot+=15:
		rot-=15;

	if(bDive)
	{
		bDiveDirection?
			Vel.y-=0.25f:
			Vel.y+=0.25f;
		Vel.y*=0.95f;
	}
	else
	{
		if(bStartMovement && mSphere.p.x < gpEngine->mPlayer.mSphere.p.x)
			bDive = true;
		if(!bStartMovement && mSphere.p.x > gpEngine->mPlayer.mSphere.p.x)
			bDive = true;
	}
	if(RateofFire>0)
	if(gpEngine->mTimer.GetTime()>=fLastFireTime+((float)(RateofFire+0.5f)*gpEngine->fGameDifficulty))
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
						** FireBug draw **	
						*******************/
//================================================================================================//
void FireBug::Draw(const float interp)
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
	RenderRotatedSprite(gpEngine->sprFireBug,0,p.x+12,p.y+12,25,25,rot);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void FireBug::NeedsToBeRemoved()
{
	if(Pos.y < -50 || Pos.y> 530)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool FireBug::CheckCollided(Sphere s, float damage)
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

void FireBug::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartMovement,1);
}
void FireBug::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartMovement,1);
}
int FireBug::InWater()
{
	return 0;
}