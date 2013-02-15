#include "BreakScenery.h"
#include "Engine.h"
#include "Debris.h"
#include "RandExplosion.h"
BreakScenery::BreakScenery()
{
	frame =0;
	iRotation = 0;
}
//================================================================================================//
						/*******************
						** BreakScenery spawn **	
						********************/
//================================================================================================//
void BreakScenery::Spawn(Vec2 pos)
{
	IsActive = true;
 	oPos = Pos = pos;
	fStartLife = fLife = 30;
	mSphere = Sphere((float)spr.GetWindowScale().first*0.5f,Vec2(Pos.x+(float)spr.GetWindowScale().first*0.5f,Pos.y+(float)spr.GetWindowScale().second*0.5f));
	if(frame==3)
		mSphere = Sphere(20,Vec2(Pos.x+(float)spr.GetWindowScale().first*0.5f,Pos.y+(float)spr.GetWindowScale().second*0.5f));
	iTakeDamageTicks = 0;
	internalFrame = 0;
}
//================================================================================================//
						/*******************
						** BreakScenery Clone **	
						********************/
//================================================================================================//
BreakScenery* BreakScenery::Clone()const
{
	return new BreakScenery(*this);
}
//================================================================================================//
						/********************
						** BreakScenery update **	
						*********************/
//================================================================================================//
void BreakScenery::Update()
{
	iTakeDamageTicks--;
	if(frame == 3)//this is a booster
	{
		internalFrame<7?internalFrame+=0.35f:internalFrame-=7;
	}
}
//================================================================================================//
						/******************
						** BreakScenery draw **	
						*******************/
//================================================================================================//
void BreakScenery::Draw(const float interp)
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

	if(frame!=3)
	{
		switch(iRotation)
		{
		case 0://noraml
			RenderSprite(spr,(int)frame,p.x,p.y,(float)spr.GetWindowScale().first,(float)spr.GetWindowScale().second);
			break;
		case 1://flipped horizontally
			RenderSprite(spr,(int)frame,p.x+(float)spr.GetWindowScale().first,p.y,-(float)spr.GetWindowScale().first,(float)spr.GetWindowScale().second);
			break;
		case 2://flipped vertically
			RenderSprite(spr,(int)frame,p.x,p.y+(float)spr.GetWindowScale().second,(float)spr.GetWindowScale().first,-(float)spr.GetWindowScale().second);
			break;
		case 3://flipped vertically & horizontally
			RenderSprite(spr,(int)frame,p.x+(float)spr.GetWindowScale().first,p.y+(float)spr.GetWindowScale().second,-(float)spr.GetWindowScale().first,-(float)spr.GetWindowScale().second);
			break;
		}
	}
	else
	{
		switch(iRotation)
		{
		case 0://noraml
			RenderRotatedSprite(gpEngine->sprThruster,(int)internalFrame,mSphere.p.x,mSphere.p.y,34,34,135);
			break;
		case 1://flipped horizontally
			RenderRotatedSprite(gpEngine->sprThruster,(int)internalFrame,mSphere.p.x,mSphere.p.y,34,34,45);
			break;
		case 2://flipped vertically
			RenderRotatedSprite(gpEngine->sprThruster,(int)internalFrame,mSphere.p.x,mSphere.p.y,34,34,-135);
			break;
		case 3://flipped vertically & horizontally
			RenderRotatedSprite(gpEngine->sprThruster,(int)internalFrame,mSphere.p.x,mSphere.p.y,34,34,-45);
			break;
		}
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	if(frame!=3)
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,0,1);
		GeoDraw2D::DrawBox(Box((float)spr.GetWindowScale().first,(float)spr.GetWindowScale().second,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.5f))));
		GeoDraw2D::DrawSphere(mSphere, 15);
	}//*/
/*	if(frame==3)
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,0,1);
		GeoDraw2D::DrawSphere(mSphere, 15);
	}//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void BreakScenery::NeedsToBeRemoved()
{
	if(Pos.x+(float)spr.GetWindowScale().first<gpEngine->Scroll)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool BreakScenery::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;

	if(frame!=3)
	{
		Box box((float)spr.GetWindowScale().first,(float)spr.GetWindowScale().second,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.5f)));
		if(!Collision::SphereBoxOverlap(s,box))
			return false;
	}
	else
	{
		if(!Collision::SphereSphereOverlap(s,mSphere))
			return false;
	}
//	if(damage<fLife)//not enough power to break the ent, but still hit it.
//		return true;

	fLife -= damage;
	if(fLife<=0)
	{
		IsActive = false;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.5f)),170,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.5f)),300,0.5f, (float)(RotAngle),true);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2((float)(spr.GetWindowScale().first*0.25f),(float)(spr.GetWindowScale().second*0.5f)),130,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2((float)(spr.GetWindowScale().first*0.75f),(float)(spr.GetWindowScale().second*0.5f)),130,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.25f)),130,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2((float)(spr.GetWindowScale().first*0.5f),(float)(spr.GetWindowScale().second*0.75f)),130,0.75f, (float)(RotAngle),false);
		SND_LARGEEXPLODE2;
		gpEngine->mPlayer.iScore+=100;
		gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.5f);
		gpEngine->mFade.SetSrcColor(1,1,1,0.75f);
		gpEngine->mFade.SetDstColor(1,1,1,0);
		float debRot = 0;
		float incRot = 360.0f/20.0f;
		Vec2 debUp,debRight;
		for(int n=0;n<20;n++)
		{
			Debris* deb = new Debris;
			UTIL_Misc::MakeVectors(debRot,debUp,debRight);
			deb->Spawn(mSphere.p);
			deb->Vel = debRight * ((float)(rand()%4)+1);
			gpEngine->GiveEntityToList(deb);
			debRot+=incRot;
		}
		RandExplosion* rE = new RandExplosion;
		rE->Spawn(mSphere.p);
		gpEngine->GiveEntityToList(rE);
	}
//	iTakeDamageTicks = 3;
	return true;
}

void BreakScenery::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&frame,1);
	fIO.ReadBinary(&iRotation,1);
	spr = gpEngine->sprBreak1;
}
void BreakScenery::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&frame,1);
	fIO.WriteBinary(&iRotation,1);
}
int BreakScenery::InWater()
{
	return 0;
}