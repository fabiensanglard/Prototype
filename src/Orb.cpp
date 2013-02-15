#include "Orb.h"
#include "Engine.h"
//================================================================================================//
						/**************
						** Spawn Orb **	
						***************/
//================================================================================================//
void Orb::Spawn(Vec2 pos)
{
	oPos = Pos = pos;
	frame = 0;
	bLockState = true;
	for(int n=0;n<6;n++)
	{
		mChain[n]=mOChain[n]=Pos;
	}
	rot = 270;
	fPulsate = 0;
	fGlowRotation = 0;
	iWeaponType = 0;
	mWaterticks = 0;
}
//================================================================================================//
				/****************************************************************
				** Give target to track, should pass this function after spawn **	
				*****************************************************************/
//================================================================================================//
void Orb::GiveTarget(Vec2* target)
{
	pTarget = target;
	mLockOffset.x = 48;
	mLockOffset.y = 0;

	mChain[0]=Pos+mLockOffset+Vec2(0,32);
	mChain[1]=Pos+mLockOffset+Vec2(1,32+4);
	mChain[2]=Pos+mLockOffset+Vec2(11,32+9);
	mChain[3]=Pos+mLockOffset+Vec2(22,32+11);
	mChain[4]=Pos+mLockOffset+Vec2(32,32+10);
	mChain[5]=Pos+mLockOffset+Vec2(41,32);
	for(int n=0;n<6;n++)
	{
		mOChain[n] = mChain[n] + Vec2(0,0.5f);
		mChainOffset[n] = mChain[n]-*pTarget;
	}
	oPos = Pos = mChain[5]-Vec2(32,32);
	mLockOffset = Pos - *pTarget;
} 
//================================================================================================//
						/***************
						** Orb update **	
						****************/
//================================================================================================//
void Orb::Update()
{
	InWater();

	if(bLockState)//locked mode
	{
		oPos = Pos;
		Pos = *pTarget + mLockOffset;

		for(int n=0;n<6;n++)
		{
			mOChain[n] = mChain[n];
			mChain[n] = *pTarget + mChainOffset[n];
		}
	}
	else//chain mode
	{
		Vec2 r;
		mOChain[0]=mChain[0];
		Vec2 p = *pTarget;
		p += Vec2(32+SCROLL_SPEED,32);
		float d;
		d=mChain[0].Distance(p);
		if(d>10)
		{
			d-=10;
			r = Normalize(mChain[0]-p);
			mChain[0] -= r*(d*0.5f);
		}
		for(int n=1;n<6;n++)
		{
			mOChain[n]=mChain[n];
			mChain[n].x+=SCROLL_SPEED;
			d=mChain[n].Distance(mChain[n-1]);
			if(d>10)
			{
				d-=10;
				r = Normalize(mChain[n]-mChain[n-1]);
				mChain[n] -= r*(d*0.5f);
			}
		}
		Vec2 ray(Normalize(mChain[4]-mChain[5]));
		oPos = mOChain[5] - Vec2(32,32) - ray*16;
		Pos = mChain[5] - Vec2(32,32) - ray*16;
	}
	frame+=0.5f;
	if(frame>15)frame-=15;

	mSphere = Sphere(15,Pos+Vec2(32,32));
	
	fPulsate += 0.15f;
	UTIL_Misc::Clamp(fPulsate,0,360);
	fGlowRotation += 1.5f;
	UTIL_Misc::Wrap(fGlowRotation,360);
}
//================================================================================================//
						/************************
						** Virtual constructor **	
						*************************/
//================================================================================================//
Orb* Orb::Clone()const
{
	return new Orb(*this);
}
//================================================================================================//
						/***********
						** Render **	
						************/
//================================================================================================//
void Orb::Draw(const float interp)
{
	//orb
	float x=UTIL_Misc::Interpolate(Pos.x,oPos.x,interp);
	float y=UTIL_Misc::Interpolate(Pos.y,oPos.y,interp);
	float s = sin(fPulsate);
	glColor4f(1,1,1,1);
	if(gpEngine->mPlayer.bSpawnOrb)
		glColor4f(0,1,1,(float)gpEngine->mPlayer.iSpawnOrbTicks/25.0f);

	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	RenderRotatedSprite(gpEngine->sprOrb1,(int)frame+(16*iWeaponType),x+32,y+32,20,20,rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void Orb::NeedsToBeRemoved()
{

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Orb::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

//	fLife -= damage;
	return true;
}

void Orb::LoadFromFile(CFileIO &fIO)
{
}
void Orb::WriteToFile(CFileIO &fIO)
{
}
int Orb::InWater()
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