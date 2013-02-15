#include "BrainBoss.h"
#include "BrainCell.h"
#include "Engine.h"

BrainBoss::BrainBoss()
{

}
//================================================================================================//
						/*******************
						** BrainBoss spawn **	
						********************/
//================================================================================================//
void BrainBoss::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 500;
	mBodySphere = Sphere(60,pos+Vec2(128,128));
	mSphere = Sphere(20,pos+Vec2(68,128));
	iTakeDamageTicks = 0;

	iAttackWave = 0;
	iAttackTicks = 0;
	bJustSpawned = true;
	fRateOfSpawn = gpEngine->mTimer.GetTime();

	bIsDead = false;
	iDeathTicks = 0;
}
//================================================================================================//
						/*******************
						** BrainBoss Clone **	
						********************/
//================================================================================================//
BrainBoss* BrainBoss::Clone()const
{
	return new BrainBoss(*this);
}
//================================================================================================//
						/********************
						** BrainBoss update **	
						*********************/
//================================================================================================//
void BrainBoss::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;

	if(bIsDead)
	{
		iDeathTicks++;
		if(iDeathTicks>150)
		{
			IsActive = false;
			gpEngine->StartCompleted();
		}
		return;
	}
	iAttackTicks++;
	if(bJustSpawned)
	{
		if(iAttackTicks>50)
		{
			bJustSpawned = false;
			iAttackTicks = 0;
		}
		return;
	}

	if(iAttackWave==0)
	{
		float r = UTIL_Misc::TimeRamp(0,2*PI,0,150,(float)iAttackTicks);
		Pos.x-=sin(r)*11;
		Pos.y+=cos(r)*9.5f;
		if(iAttackTicks>=150)
		{
			iAttackTicks=0;
			iAttackWave=1;
			fRateOfFire = gpEngine->mTimer.GetTime();
		}
		if(gpEngine->mTimer.GetTime()>fRateOfSpawn)
		{
			fRateOfSpawn = gpEngine->mTimer.GetTime()+0.25f;
			BrainCell* cell = new BrainCell;
			cell->Spawn(mSphere.p);
			gpEngine->GiveEntityToList((Entity*)cell);
		}
	}
	if(iAttackWave==1)
	{
		if(iAttackTicks>=100 && iAttackTicks<=130|| iAttackTicks<=30)
		if(gpEngine->mTimer.GetTime()>fRateOfFire)
		{
			fRateOfFire = gpEngine->mTimer.GetTime()+0.1f;
			Bullet b;
			b.pSpawn = Spawn_PowerBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BossPowerBullet;
			b.pCollide = Collide_PowerBullet;
			Vec2 p(mSphere.p);
			Vec2 v = FireAtTarget(p,gpEngine->mPlayer.mSphere.p,7.5f);
			(*b.pSpawn)(b,p,v);
			Line l1(gpEngine->mPlayer.mSphere.p,p);
			Line l2(p,Vec2(p.x,p.y+Distance(gpEngine->mPlayer.mSphere.p,p)));
			if(p.x>gpEngine->mPlayer.mSphere.p.x)
				b.rot = -(RAD2DEG(l1.Angle(l2))-90);
			else
				b.rot = (RAD2DEG(l1.Angle(l2))+90);
			gpEngine->SpawnBullet(b,false);
			SND_LASER4;
		}
	


		if(iAttackTicks>=200)
		{
			iAttackTicks=0;
			iAttackWave=0;
			fRateOfSpawn = gpEngine->mTimer.GetTime();
		}
	}
	mBodySphere.p = Pos+Vec2(128,128);
}
//================================================================================================//
						/******************
						** BrainBoss draw **	
						*******************/
//================================================================================================//
void BrainBoss::Draw(const float interp)
{
	if(bIsDead)
		return;

	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);

	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossBrain);
	UTIL_GL::GL2D::DrawTextureQuad(Pos.x,Pos.y+256,Pos.x+256,Pos.y);
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
void BrainBoss::NeedsToBeRemoved()
{

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool BrainBoss::CheckCollided(Sphere s, float damage)
{
	if(!IsActive || bIsDead)
		return false;

	if(Collision::SphereSphereOverlap(s,mSphere))
	{
		fLife -= damage;
		if(fLife<=0)
		{
			bIsDead = true;
			int RotAngle = rand()%360;
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,290,0.75f, (float)(RotAngle),false);
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,220,0.5f, (float)(RotAngle),true);
			SND_SMALLEXPLODE;
			gpEngine->mPlayer.iScore+=500;
			gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.5f);
			gpEngine->mFade.SetSrcColor(1,0,0,1);
			gpEngine->mFade.SetDstColor(1,0,0,0);

			list<Entity*>::iterator i;
			for(i=gpEngine->mGameEnts.begin();i!=gpEngine->mGameEnts.end();i++)
			{
				if((*i)==this)
					continue;
				(*i)->CheckCollided((*i)->mSphere,100000);//kill all ents
			}
			gpEngine->bWimpMode = true;
		}
		iTakeDamageTicks = 3;
		return true;
	}
	if(Collision::SphereSphereOverlap(s,mBodySphere))
	{
		return true;
	}
	return false;
}

void BrainBoss::LoadFromFile(CFileIO &fIO)
{

}
void BrainBoss::WriteToFile(CFileIO &fIO)
{

}
int BrainBoss::InWater()
{
	return 0;
}