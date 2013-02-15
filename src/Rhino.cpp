#include "Rhino.h"
#include "Engine.h"
#include "Debris.h"

Rhino::Rhino()
{
	bStartDirection = false;//left
	RateofFire = 0;
	iStartOnScreen = 0;
}
//================================================================================================//
						/*******************
						** Rhino spawn **	
						********************/
//================================================================================================//
void Rhino::Spawn(Vec2 pos)
{
	IsActive = true;
	if(iStartOnScreen==2)
		pos.y=-32;
	oPos = Pos = Vec2(pos.x,pos.y-32);
	frame = 0;
	fLastFireTime = gpEngine->mTimer.GetTime();
	fStartLife = fLife = 10;
	mSphere = Sphere(24,pos+Vec2(24,8));
	iTakeDamageTicks = 0;
	bIsTurning = false;
	bIsAiming = false;
	bDropAim = false;
	nBulletsFired = 0;
	bDirection = bStartDirection;
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
}
//================================================================================================//
						/*******************
						** Rhino Clone **	
						********************/
//================================================================================================//
Rhino* Rhino::Clone()const
{
	return new Rhino(*this);
}
//================================================================================================//
						/********************
						** Rhino update **	
						*********************/
//================================================================================================//
void Rhino::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	if(!gpEngine->bRecording || gpEngine->bRecording && gpEngine->pRecordEnt != this)
		PlayBack();

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
	else if(bIsAiming)
	{
		if(bDropAim)
		{
			frame<0? frame=0: frame-=0.5f;
			if(frame==0)
			{
				bIsAiming = false;
				nBulletsFired = 0;
				bDropAim = false;
			}
		}
		else
		{
			frame>15? frame=15: frame+=0.5f;

			if(frame==15)
			{
				if(gpEngine->mTimer.GetTime()>=fLastFireTime+((float)0.25f*gpEngine->fGameDifficulty))
				{
					SND_LASER2;
					fLastFireTime = gpEngine->mTimer.GetTime();
					Bullet b;
					b.pSpawn = Spawn_GravityBullet;
					b.pUpdate = Update_GravityBullet;
					b.pRender = Render_GravityBullet;
					b.pCollide = Collide_GravityBullet;
					bDirection?
						(*b.pSpawn)(b,Vec2(Pos.x+32,Pos.y),Vec2(0.15f,-0.5f)*14):
						(*b.pSpawn)(b,Pos,Vec2(-0.15f,-0.5f)*14);
					bDirection?
						b.user2 = 0: b.user2 = 1;
					gpEngine->SpawnBullet(b,false);
					nBulletsFired++;
					if(nBulletsFired==10)
						bDropAim = true;
				}
			}
		}
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
			bIsAiming = true;
		}
	}
	//make fall
	Pos.y+=4.5f;
	if((Pos.y+64)>0 && ((Pos.y+64)/32)<15 && Pos.x>0 && (Pos.x/32)<400)
	{
		int ID = gpEngine->mTilesLayer1[(int)(Pos.x+32)/32][(int)(Pos.y+64)/32].ID;
		if(ID>=0 && ID<512)
			Pos.y = oPos.y;
//		Pos.y = (float)((int)(oPos.y/32)*32);
	}


}
//================================================================================================//
						/******************
						** ladybird draw **	
						*******************/
//================================================================================================//
void Rhino::Draw(const float interp)
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
	p.y+=4;
	if(!bIsTurning && !bIsAiming)
	{
		bDirection?
		RenderSprite(gpEngine->sprRhinoWalk,(int)frame,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprRhinoWalk,(int)frame,p.x+64,p.y,-64,64);
	}
	else if(bIsTurning)
	{
		bDirection?
		RenderSprite(gpEngine->sprRhinoTurn,(int)frame,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprRhinoTurn,(int)frame,p.x+64,p.y,-64,64);
	}
	else if(bIsAiming)
	{
		bDirection?
		RenderSprite(gpEngine->sprRhinoAim,(int)frame,p.x,p.y,64,64):
			RenderSprite(gpEngine->sprRhinoAim,(int)frame,p.x+64,p.y,-64,64);
	}
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
void Rhino::NeedsToBeRemoved()
{
	if(Pos.x+64<gpEngine->Scroll-100 || Pos.y>600)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Rhino::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(32,32),90,0.75f, (float)(rand()%360),false);
		SND_LARGEEXPLODE;
		gpEngine->mPlayer.iScore+=50;

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

void Rhino::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bStartDirection,1);
	fIO.ReadBinary(&iStartOnScreen,1);
	ReadBufferFromFile(fIO);
}
void Rhino::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bStartDirection,1);
	fIO.WriteBinary(&iStartOnScreen,1);
	WriteBufferToFile(fIO);
}
int Rhino::InWater()
{
	return 0;
}