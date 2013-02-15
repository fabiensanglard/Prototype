#include "StarBurst.h"
#include "Engine.h"
#include "Debris.h"
StarBurst::StarBurst()
{
	sinus = 180;
	RateofFire = 0;
}
//================================================================================================//
						/*******************
						** StarBurst spawn **	
						********************/
//================================================================================================//
void StarBurst::Spawn(Vec2 pos)
{
	IsActive = true;
	pos.x = gpEngine->Scroll+640;
	oPos = Pos = pos;
	frame = 0;
	fLastFireTime = gpEngine->mTimer.GetTime()+RateofFire;
	fStartLife = fLife = 20;
	mSphere = Sphere(20,pos+Vec2(34,28));
	iTakeDamageTicks = 0;
	sinus = 0;
	iTimesFired = 0;
}
//================================================================================================//
						/*******************
						** StarBurst Clone **	
						********************/
//================================================================================================//
StarBurst* StarBurst::Clone()const
{
	return new StarBurst(*this);
}
//================================================================================================//
						/********************
						** StarBurst update **	
						*********************/
//================================================================================================//
void StarBurst::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	sinus+=0.05f;
	bStartMovement?
		Pos.y-=sin(sinus)*3:
		Pos.y+=sin(sinus)*3;
	frame<15? frame+=0.75f: frame-=15;

	if(RateofFire>0)
	if(gpEngine->mTimer.GetTime()>=fLastFireTime)
	{
		iTimesFired++;
		if(iTimesFired>(9/gpEngine->fGameDifficulty))
		{
			fLastFireTime = gpEngine->mTimer.GetTime()+(RateofFire*gpEngine->fGameDifficulty);
			iTimesFired = 0;
		}
		else
			fLastFireTime = gpEngine->mTimer.GetTime()+0.1f;


		Bullet b;
		b.pSpawn = Spawn_BasicBullet;
		b.pUpdate = Update_BasicBullet;
		b.pRender = Render_BasicBullet;
		b.pCollide = Collide_BasicBullet;
		(*b.pSpawn)(b,Pos+Vec2(32,32),Vec2(-5,0));
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Pos+Vec2(32,32),Vec2(-5,-2));
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Pos+Vec2(32,32),Vec2(-5,2));
		gpEngine->SpawnBullet(b,false);
		SND_LASER2;
	}
}
//================================================================================================//
						/******************
						** StarBurst draw **	
						*******************/
//================================================================================================//
void StarBurst::Draw(const float interp)
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

	RenderSprite(gpEngine->sprStarBurst,(int)frame,p.x,p.y,64,64);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 25);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void StarBurst::NeedsToBeRemoved()
{
	if(Pos.x+(mSphere.r*2)<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool StarBurst::CheckCollided(Sphere s, float damage)
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
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,24),140,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Pos+Vec2(24,24),170,0.5f, (float)(RotAngle),true);
		SND_MEDEXPLODE;
		gpEngine->mPlayer.iScore+=200;

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

void StarBurst::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartMovement,1);
}
void StarBurst::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartMovement,1);
}
int StarBurst::InWater()
{
	return 0;
}