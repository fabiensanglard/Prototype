#include "PowerDroid.h"
#include "Engine.h"
#include "Debris.h"
PowerDroid::PowerDroid()
{
	bStartDirection = false;//left
	iPowerUp = 0;
}
//================================================================================================//
						/*******************
						** PowerDroid spawn **	
						********************/
//================================================================================================//
void PowerDroid::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 1;
	mSphere = Sphere(16,pos+Vec2(28,24));
	iTakeDamageTicks = 0;
	bIsTurning = false;
	bIsFlying = true;
	Vel = Vec2(0,0);
	bDirection = bStartDirection;
	bDirection?
		Vel.x=2.5f:Vel.x=-2.5f;
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** PowerDroid Clone **	
						********************/
//================================================================================================//
PowerDroid* PowerDroid::Clone()const
{
	return new PowerDroid(*this);
}
//================================================================================================//
						/********************
						** PowerDroid update **	
						*********************/
//================================================================================================//
void PowerDroid::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	if(!gpEngine->bRecording || gpEngine->bRecording && gpEngine->pRecordEnt != this)
		PlayBack();

	InWater();

	if(bIsTurning)
	{
		frame+=0.25f;
		if(frame>15)
		{
			bDirection = !bDirection;
			bIsTurning = false;
			frame = 0;
		}
	}
	else if(bIsFlying)
	{
		if(KEY_ATTACK2.state == PRESSED)
		{
			
			Vel.y-=0.15f;
			if(bDirection)
				Vel.x=2.5f;
			else
				Vel.x=-2.5f;

		}
		Pos.x+=Vel.x;
		frame<15? frame+=0.5f:frame=0;
	}
	else//standing or walking
	{
		if(bDirection)//right
		{
			if(KEY_RIGHT.state == PRESSED)
			{
				frame>15? frame-=15: frame+=0.25f; 
				Pos.x+=1.5f;
			}
			if(KEY_LEFT.state == JUST_PRESSED)
			{
				bIsTurning = true;
				frame = 0;
			}
		}
		else//left
		{
			if(KEY_LEFT.state == PRESSED)
			{
				frame>15? frame-=15: frame+=0.25f; 
				Pos.x-=1.5f;
			}
			if(KEY_RIGHT.state == JUST_PRESSED)
			{
				bIsTurning = true;
				frame = 0;
			}
		}
		if(KEY_ATTACK2.state == JUST_PRESSED)
		{
			frame = 0;
			bIsFlying = true;
			Pos.y-=3;
		}
	}
	//make fall
	Vel.y+=0.075f;
	Pos.y+=Vel.y;
	if((Pos.y+64)>0 && ((Pos.y+64)/32)<15 && Pos.x>0 && (Pos.x/32)<400)
	{
		int ID;
		if(bIsFlying)//if its flying make it stop if hits a wall
		{
			bool bHit=false;
			bDirection?
			ID = gpEngine->mTilesLayer1[(int)((Pos.x+32)/32)+1][(int)(Pos.y+32)/32].ID:
			ID = gpEngine->mTilesLayer1[(int)(Pos.x)/32][(int)(Pos.y+32)/32].ID;
			if(ID>=0 && ID<512)
			{
				Vel.x=0;
				bHit= true;
			}
			ID = gpEngine->mTilesLayer1[(int)(Pos.x+32)/32][(int)(Pos.y)/32].ID;
			if(ID>=0 && ID<512)
			{
				Vel.y=+1;
				bHit = true;
			}
	//		if(bHit)
	//			return;
		}

		ID = gpEngine->mTilesLayer1[(int)(Pos.x+32)/32][(int)(Pos.y+64)/32].ID;
		if(ID>=0 && ID<512)
		{
			Vel.y=0;
			Vel.x=0;
			Pos.y = oPos.y;
			if(bIsFlying)
			{
				bIsFlying = false;
				frame =0;
			}
		}
	}


}
//================================================================================================//
						/******************
						** PowerDroid draw **	
						*******************/
//================================================================================================//
void PowerDroid::Draw(const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	p.y+=4;
	if(bIsFlying)
	{
		bDirection?
		RenderSprite(gpEngine->sprPowerDroid,(int)frame,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprPowerDroid,(int)frame,p.x+64,p.y,-64,64);
	}
	else if(bIsTurning)
	{
		bDirection?
		RenderSprite(gpEngine->sprPowerDroid,(int)frame+32,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprPowerDroid,(int)frame+32,p.x+64,p.y,-64,64);
	}
	else
	{
		bDirection?
		RenderSprite(gpEngine->sprPowerDroid,(int)frame+16,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprPowerDroid,(int)frame+16,p.x+64,p.y,-64,64);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDisable(GL_BLEND);
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
/*	Sphere s2(mSphere.r,Vec2(mSphere.p.x,mSphere.p.y+22));
	GeoDraw2D::DrawSphere(s2, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void PowerDroid::NeedsToBeRemoved()
{
	if(Pos.x+64<gpEngine->Scroll)
		IsActive = false;
	if(Pos.y>600)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool PowerDroid::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	Sphere s2(mSphere.r,Vec2(mSphere.p.x,mSphere.p.y+22));
	if(!Collision::SphereSphereOverlap(s,mSphere) && !Collision::SphereSphereOverlap(s,s2))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(32,32),90,0.75f, (float)(rand()%360),false);
		PowerUp* p = new PowerUp;
		p->iType = iPowerUp;
		p->Spawn(Pos);
		gpEngine->GiveEntityToList(p);
		SND_MEDEXPLODE;
		gpEngine->mPlayer.iScore+=30;

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

void PowerDroid::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bStartDirection,1);
	fIO.ReadBinary(&iPowerUp,1);
	ReadBufferFromFile(fIO);
}
void PowerDroid::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bStartDirection,1);
	fIO.WriteBinary(&iPowerUp,1);
	WriteBufferToFile(fIO);
}
int PowerDroid::InWater()
{
	mWaterticks++;
	if(mWaterticks<5)
		return 0;
	mWaterticks=0;
	int x = (int)(mSphere.p.x/32);
	int y = (int)(mSphere.p.y/32);
	if(x<0 || x>399 || y<0 || y>15)
		return 0;
	if(gpEngine->mTilesLayer1[x][y].ID==1018)//water surface
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
	return 0;
}