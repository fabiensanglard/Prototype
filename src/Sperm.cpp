#include "Sperm.h"
#include "Engine.h"

Sperm::Sperm()
{

}
//================================================================================================//
						/*******************
						** Sperm spawn **	
						********************/
//================================================================================================//
void Sperm::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 1;
	mSphere = Sphere(5,pos+Vec2(4,4));
	iTakeDamageTicks = 0;
	fRotation = 0;
	iLifeTicks=  0;
}
//================================================================================================//
						/*******************
						** Sperm Clone **	
						********************/
//================================================================================================//
Sperm* Sperm::Clone()const
{
	return new Sperm(*this);
}
//================================================================================================//
						/********************
						** Sperm update **	
						*********************/
//================================================================================================//
void Sperm::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	frame<5?frame+=0.25f:frame-=5;

	//chase player

	Vec2 target = gpEngine->mPlayer.mSphere.p;
	Vec2 u,r;
	UTIL_Misc::MakeVectors(fRotation,u,r);
	Vel = r * 2;
		Vec2 toTarget;
		toTarget = Normalize(mSphere.p-target);
		if(Dot(u,toTarget)<0)
			fRotation -= (1.0f - Dot(r,toTarget))*2.5f;
		else
			fRotation += (1.0f - Dot(r,toTarget))*2.5f;

	Pos += Vel;

	iLifeTicks++;
	if(iLifeTicks>400)
	{
		IsActive = false;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,70,0.75f, (float)(RotAngle),false);
		SND_SMALLEXPLODE;
	}
}
//================================================================================================//
						/******************
						** Sperm draw **	
						*******************/
//================================================================================================//
void Sperm::Draw(const float interp)
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
	RenderRotatedSprite(gpEngine->sprBossSperm,(int)frame,p.x,p.y,16,16,fRotation-90);

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
void Sperm::NeedsToBeRemoved()
{

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Sperm::CheckCollided(Sphere s, float damage)
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
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,70,0.75f, (float)(RotAngle),false);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=25;
	}
	iTakeDamageTicks = 3;
	return true;
}

void Sperm::LoadFromFile(CFileIO &fIO)
{

}
void Sperm::WriteToFile(CFileIO &fIO)
{

}
int Sperm::InWater()
{
	return 0;
}