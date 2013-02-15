#include "Serpent.h"
#include "Engine.h"
Serpent::Serpent()
{
	IsActive = false;
	fRotation = 0;
	RateofFire = 0;
	IsDead = false;
	IsVisible = true;
}
//================================================================================================//
						/*******************
						** Serpent spawn **	
						********************/
//================================================================================================//
void Serpent::Spawn(Vec2 pos)
{
	IsActive = true;
	bStartDirection? frame = 11: frame = 3;
	fLastFireTime = gpEngine->mTimer.GetTime();
	bStartDirection? fRotation = -90: fRotation = -270;
	bStartDirection? pos.y = 512: pos.y=-32;
	oPos = Pos = pos;

	fStartRotation = fRotation;
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
	mSections.clear();

	iTickSection = 10;
	StartPos = pos;
	IsHead = true;
	for(int n = SERPENT_LENGTH; n>0; n--)
	{
		Serpent s;
		s.bStartDirection = this->bStartDirection;
		s.mKeyBuffer = this->mKeyBuffer;
		s.fStartRotation = this->fStartRotation;
		s.IsDead = false;
		s.IsActive = false;
		mSections.push_back(s);
	}
	iCurSection = SERPENT_LENGTH;
	fStartLife = fLife = 30;
	mSphere = Sphere(25,pos+Vec2(60,10));
	IsVisible = true;
	IsDead = false;
	iTakeDamageTicks = 0;
	mWaterticks=0;
}
//================================================================================================//
						/***********************
						** Serpent body spawn **	
						************************/
//================================================================================================//
void Serpent::SpawnSection(Vec2 pos)
{
	if(IsDead)
		return;
	IsActive = true;
	oPos = Pos = pos;
	bStartDirection? frame = 11: frame = 3;
	fLastFireTime = gpEngine->mTimer.GetTime();
	bStartDirection? fRotation = -90: fRotation = -270;
	fRotation = fStartRotation;
	SelfControl();
	IsHead = false;
	mSphere = Sphere(25,pos+Vec2(60,10));
	IsVisible = true;
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** Serpent Clone **	
						********************/
//================================================================================================//
Serpent* Serpent::Clone()const
{
	return new Serpent(*this);
}
//================================================================================================//
						/********************
						** Serpent update **	
						*********************/
//================================================================================================//
void Serpent::Update()
{
	iTakeDamageTicks--;
	if(!gpEngine->bRecording || gpEngine->bRecording && gpEngine->pRecordEnt != this)
		PlayBack();

	if(IsHead)
		InWater();
	oPos = Pos;
	if(IsDead)
		return;

	if(KEY_RIGHT.state == PRESSED)
	{
		fRotation += 1.5f;
		frame+=0.075f;
	}
	if(KEY_LEFT.state == PRESSED)
	{
		fRotation -= 1.5f;
		frame-=0.075f;
	}
	fRotation = UTIL_Misc::Wrap(fRotation,360);

	if(KEY_UP.state == PRESSED)
	{
		Vec2 u,r;
		UTIL_Misc::MakeVectors(fRotation,u,r);
		Pos+=u*2.5f;
	}
	if(frame>15)
		frame-=15;
	else if(frame<0)
		frame+=15;

	vector<Serpent>::iterator iter;
	for(iter = mSections.begin(); iter < mSections.end(); iter++)
	{
		if((*iter).IsActive)
			(*iter).PreUpdate();
	}

	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	iTickSection--;
	if(IsHead && iTickSection == 0 || IsHead && iTickSection==7 && iCurSection == SERPENT_LENGTH)
	{
		if(iCurSection>=0)
		{
			mSections[iCurSection].SpawnSection(StartPos);
			iCurSection--;
			iTickSection = 10;
		}
	}

	//kill off each section
	if(IsHead && !IsVisible)
	{
		iTickToDeath--;
		if(iTickToDeath<=0)
		{
			if(iCurSegDeath>=0 && mSections[iCurSegDeath].IsActive)
			{
				mSections[iCurSegDeath].IsVisible = false;
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSections[iCurSegDeath].Pos+Vec2(60,10),120,0.75f,(float)(rand()%360),false);
				if(iCurSegDeath>0)
					SND_MEDEXPLODE;
				else
					SND_LARGEEXPLODE2;
				iCurSegDeath--;
			}
			else
				IsActive = false;
			iTickToDeath=5;
		}
	}

	//fire from tail
	if(IsHead && IsVisible)
	if(RateofFire>0)
	{
		if(mSections.front().IsVisible && mSections.front().IsActive)
		if(gpEngine->mTimer.GetTime()>=fLastFireTime+(0.25f*gpEngine->fGameDifficulty))
		{
			fLastFireTime = gpEngine->mTimer.GetTime();
			Bullet b;
			b.pSpawn = Spawn_BasicBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BasicBullet;
			b.pCollide = Collide_BasicBullet;
			Vec2 v = FireAtTarget(mSections.front().Pos+Vec2(48,-16),gpEngine->mPlayer.mSphere.p,3.5f);
			(*b.pSpawn)(b,mSections.front().Pos+Vec2(48,-16),v);
			gpEngine->SpawnBullet(b,false);
			SND_LASER5;
		}
	}
}
//================================================================================================//
						/******************
						** ladybird draw **	
						*******************/
//================================================================================================//
void Serpent::Draw(const float interp) 
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
//	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	Vec2 p;
	vector<Serpent>::iterator iter;	int n=0;
	for(iter = mSections.begin(); iter < mSections.end(); iter++,n++)
	{
		if(!(*iter).IsActive)
			continue;
		p = UTIL_Misc::Interpolate((*iter).Pos,(*iter).oPos,interp);
		if((*iter).IsVisible)
		{
			if(n==0)
				RenderRotatedSprite(gpEngine->sprSerpentTail,(int)(*iter).frame,p.x+64,p.y,-32,32,(*iter).fRotation+90);
			else
				RenderRotatedSprite(gpEngine->sprSerpentBody,(int)(*iter).frame,p.x+64,p.y,-32,32,(*iter).fRotation+90);
		}
/*		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,0,1);
		GeoDraw2D::DrawSphere((*iter).mSphere, 15);*/
	}

	p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	if(IsVisible)
		RenderRotatedSprite(gpEngine->sprSerpent,(int)frame,p.x+64,p.y,-40,40,fRotation+90);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);*/
}
//================================================================================================//
						/******************************
						** if its buffer has ran out **	
						*******************************/
//================================================================================================//
void Serpent::NeedsToBeRemoved()
{
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	if(!IsHead)
		return;
	if(Pos.x<(gpEngine->Scroll-100))
	{
		vector<Serpent>::iterator iter;
		for(iter = mSections.begin(); iter < mSections.end(); iter++)
		{
			if((*iter).Pos.x>=(gpEngine->Scroll-100))
				return;
		}
		IsActive = false;
	}
	if(mSections.front().mKeyBuffer.empty())
	{
		IsActive = false;
	}
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Serpent::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;


	if(!IsVisible)
		return false;
	vector<Serpent>::iterator iter;
	for(iter = mSections.begin(); iter!=mSections.end(); iter++)
	{
		if((*iter).IsVisible)
			if(Collision::SphereSphereOverlap(s,(*iter).mSphere))
				return true;
	}


	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(60,10),120,0.75f,(float)(RotAngle),false);
		if(IsHead)
		{
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Pos+Vec2(24,24),150,0.5f, (float)(RotAngle),true);
			SND_LARGEEXPLODE2;
			gpEngine->mPlayer.iScore+=500;
//			IsDead = true;
		}

		IsVisible = false;
		iCurSegDeath = SERPENT_LENGTH-1;
		iTickToDeath = 5;
		for(iter = mSections.begin(); iter!=mSections.end(); iter++)
		{
			(*iter).IsDead = true;
		}
	}
	iTakeDamageTicks = 3;
	return true;
}

void Serpent::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&RateofFire,1);
	fIO.ReadBinary(&bStartDirection,1);
	ReadBufferFromFile(fIO);
}
void Serpent::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&RateofFire,1);
	fIO.WriteBinary(&bStartDirection,1);
	WriteBufferToFile(fIO);
}
int Serpent::InWater()
{
	mWaterticks++;
	if(mWaterticks<5)
		return 0;
	mWaterticks=0;
	int x = (int)(mSphere.p.x/32);
	int y = (int)(mSphere.p.y/32);
	if(gpEngine->mTilesLayer1[x][y].ID==1018)//water surface
	{
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x+32,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
	}
	vector<Serpent>::iterator iter;
	for(iter = mSections.begin(); iter!=mSections.end(); iter++)
	{
		if((*iter).IsVisible)
		{
			x = (int)(*iter).mSphere.p.x/32;
			y = (int)(*iter).mSphere.p.y/32;
			if(gpEngine->mTilesLayer1[x][y].ID==1018)//water surface
			{
				gpEngine->SpawnSpriteEffect(1,Vec2((*iter).mSphere.p.x,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
				gpEngine->SpawnSpriteEffect(1,Vec2((*iter).mSphere.p.x+32,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
			}
		}
	}
	return 0;
}