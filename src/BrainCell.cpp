#include "BrainCell.h"
#include "Sperm.h"
#include "Engine.h"

BrainCell::BrainCell()
{

}
//================================================================================================//
						/*******************
						** BrainCell spawn **	
						********************/
//================================================================================================//
void BrainCell::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 3;
	mSphere = Sphere(20,pos+Vec2(32,32));
	iTakeDamageTicks = 0;

	iAttackTicks = 0;
	pulsate = 0;
}
//================================================================================================//
						/*******************
						** BrainCell Clone **	
						********************/
//================================================================================================//
BrainCell* BrainCell::Clone()const
{
	return new BrainCell(*this);
}
//================================================================================================//
						/********************
						** BrainCell update **	
						*********************/
//================================================================================================//
void BrainCell::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;

	iAttackTicks++;

	if(iAttackTicks>=150)
	{
		IsActive = false;
		iAttackTicks=0;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,120,0.75f, (float)(RotAngle),false);
		SND_SMALLEXPLODE;
		int maxSperm;
		if(gpEngine->fGameDifficulty==3)
			maxSperm = 1;
		else if(gpEngine->fGameDifficulty==1)
			maxSperm = 3;
		else
			maxSperm = 2;
		float rot = (float)360/maxSperm;
		float r=0;
		Vec2 up,right;
		for(int n=0;n<maxSperm;n++)
		{
			Sperm* psperm = new Sperm;
			UTIL_Misc::MakeVectors(r,up,right);
			psperm->Spawn(mSphere.p += right * 10);
			psperm->fRotation = r;
			gpEngine->GiveEntityToList((Entity*)psperm);
			r+=rot;
		}
	}
	pulsate+=0.1f;
}
//================================================================================================//
						/******************
						** BrainCell draw **	
						*******************/
//================================================================================================//
void BrainCell::Draw(const float interp)
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
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossCell);
	float pulse = sin(pulsate)*5;
	UTIL_GL::GL2D::DrawTextureQuad(Pos.x-pulse,Pos.y+64+pulse,Pos.x+64+pulse,Pos.y-pulse);
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
void BrainCell::NeedsToBeRemoved()
{

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool BrainCell::CheckCollided(Sphere s, float damage)
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
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,120,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,120,0.5f, (float)(RotAngle),true);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=50;
	}
	iTakeDamageTicks = 3;
	return true;
}

void BrainCell::LoadFromFile(CFileIO &fIO)
{

}
void BrainCell::WriteToFile(CFileIO &fIO)
{

}
int BrainCell::InWater()
{
	return 0;
}