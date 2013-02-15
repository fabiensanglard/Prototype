#include "Rocket.h"
#include "Engine.h"
#include "Debris.h"
Rocket::Rocket()
{
	bDirection = true;
}
//================================================================================================//
						/*****************
						** rocket spawn **	
						******************/
//================================================================================================//
void Rocket::Spawn(Vec2 pos)
{
	IsActive = true;
	pos.y-=16;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 4;
	mSphere = Sphere(15,pos+Vec2(24,24));
	bDirection?
		rotation =90: rotation = 270;
	bBlastOff = false;
	bTurnNow = false;
	bFinishedTurning = false;
	iTakeDamageTicks = 0;
}
//================================================================================================//
						/*****************
						** Rocket Clone **	
						******************/
//================================================================================================//
Rocket* Rocket::Clone()const
{
	return new Rocket(*this);
}
//================================================================================================//
						/******************
						** Rocket update **	
						*******************/
//================================================================================================//
void Rocket::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	if(bBlastOff)
	{
		frame<7? frame+=0.5f: frame-=7;
		if(!bTurnNow)
		{
			bDirection?
				Pos.y-=3.5f: Pos.y+=3.5f;

			if(bDirection)
			{
				if(Pos.y<gpEngine->mPlayer.Pos.y+5)
					bTurnNow = true;
			}
			else
			{
				if(Pos.y>gpEngine->mPlayer.Pos.y-5)
					bTurnNow = true;
			}

		}
		else
		{
			if(!bFinishedTurning)
			{
				if(bDirection)
				{
					rotation-=3.5f;
					if(rotation<=0)
					{
						rotation = 0;
						bFinishedTurning = true;
					}
				}
				else
				{
					rotation+=3.5f;
					if(rotation>=360)
					{
						rotation = 0;
						bFinishedTurning = true;
					}
				}

			}
			else
			{
				Pos.x-=5.5f;
			}
		}

	}
	else
	{
		if(gpEngine->Scroll>=BlastOffPos)
			bBlastOff = true;
	}
}
//================================================================================================//
						/******************
						** Rocket draw **	
						*******************/
//================================================================================================//
void Rocket::Draw(const float interp)
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

	RenderRotatedSprite(gpEngine->sprRocket, (int)(frame), p.x+24,p.y+24,24,24,rotation);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 5);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void Rocket::NeedsToBeRemoved()
{
	if(Pos.x+48<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Rocket::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,24),90,0.75f, (float)(rand()%360),false);
		SND_LARGEEXPLODE;
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

void Rocket::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bDirection,1);
	fIO.ReadBinary(&BlastOffPos,1);
}
void Rocket::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bDirection,1);
	fIO.WriteBinary(&BlastOffPos,1);
}
int Rocket::InWater()
{
	return 0;
}