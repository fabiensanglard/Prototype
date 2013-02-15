#include "SolarFlare.h"
#include "Engine.h"
SolarFlare::SolarFlare()
{
	IsActive = false;
	fRotation = 0;
}
//================================================================================================//
						/*******************
						** SolarFlare spawn **	
						********************/
//================================================================================================//
void SolarFlare::Spawn(Vec2 pos)
{
	IsActive = true;
	bStartDirection? frame = 11: frame = 3;
	bStartDirection? fRotation = -90: fRotation = -270;
	bStartDirection? pos.y = 512: pos.y=-32;
	oPos = Pos = pos;

	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	SelfControl();
	mSections.clear();

	iTickSection = 10;
	StartPos = pos;
	IsHead = true;
	fStartRotation = fRotation;
	for(int n = SolarFlare_LENGTH; n>0; n--)
	{
		SolarFlare s;
		s.bStartDirection = this->bStartDirection;
		s.mKeyBuffer = this->mKeyBuffer;
		s.fStartRotation = this->fStartRotation;
		s.IsActive = false;
		mSections.push_back(s);
	}
	iCurSection = SolarFlare_LENGTH;
	fStartLife = fLife = 30;
	mSphere = Sphere(25,pos+Vec2(60,10));
	iTakeDamageTicks = 0;
	mWaterticks=0;
}
//================================================================================================//
						/***********************
						** SolarFlare body spawn **	
						************************/
//================================================================================================//
void SolarFlare::SpawnSection(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	bStartDirection? frame = 11: frame = 3;
	bStartDirection? fRotation = -90: fRotation = -270;
	fRotation = fStartRotation;
	SelfControl();
	IsHead = false;
	mSphere = Sphere(25,pos+Vec2(60,10));
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** SolarFlare Clone **	
						********************/
//================================================================================================//
SolarFlare* SolarFlare::Clone()const
{
	return new SolarFlare(*this);
}
//================================================================================================//
						/********************
						** SolarFlare update **	
						*********************/
//================================================================================================//
void SolarFlare::Update()
{
	iTakeDamageTicks--;
	if(!gpEngine->bRecording || gpEngine->bRecording && gpEngine->pRecordEnt != this)
		PlayBack();

	oPos = Pos;

	frame<3?frame+=0.075f:frame-=3;
	if(KEY_RIGHT.state == PRESSED)
	{
		fRotation += 1.0f;
	}
	if(KEY_LEFT.state == PRESSED)
	{
		fRotation -= 1.0f;
	}
	fRotation = UTIL_Misc::Wrap(fRotation,360);

//	if(KEY_UP.state == PRESSED)
	{
		Vec2 u,r;
		UTIL_Misc::MakeVectors(fRotation,u,r);
		Pos+=u*4.5f;
	}
	if(frame>15)
		frame-=15;
	else if(frame<0)
		frame+=15;

	vector<SolarFlare>::iterator iter;
	for(iter = mSections.begin(); iter < mSections.end(); iter++)
	{
		if((*iter).IsActive)
			(*iter).PreUpdate();
	}

	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	iTickSection--;
	if(IsHead && iTickSection == 0 || IsHead && iTickSection==7 && iCurSection == SolarFlare_LENGTH)
	{
		if(iCurSection>=0)
		{
			mSections[iCurSection].SpawnSection(StartPos);
			iCurSection--;
			iTickSection = 10;
		}
	}
}
//================================================================================================//
						/******************
						** ladybird draw **	
						*******************/
//================================================================================================//
void SolarFlare::Draw(const float interp) 
{
//	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE);
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	Vec2 p;
	vector<SolarFlare>::iterator iter;	int n=0;
	for(iter = mSections.begin(); iter < mSections.end(); iter++,n++)
	{
		if(!(*iter).IsActive)
			continue;
		p = UTIL_Misc::Interpolate((*iter).Pos,(*iter).oPos,interp);
			if(n==0)
				RenderRotatedSprite(gpEngine->sprSolarFlare,(int)(*iter).frame,p.x+64,p.y,-80,80,(*iter).fRotation+90);
			else
				RenderRotatedSprite(gpEngine->sprSolarFlare,(int)(*iter).frame,p.x+64,p.y,-80,80,(*iter).fRotation+90);

/*		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,0,1);
		GeoDraw2D::DrawSphere((*iter).mSphere, 15);//*/
	}

	p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	RenderRotatedSprite(gpEngine->sprSolarFlare,(int)frame,p.x+64,p.y,-80,80,fRotation+90);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/******************************
						** if its buffer has ran out **	
						*******************************/
//================================================================================================//
void SolarFlare::NeedsToBeRemoved()
{
	if(gpEngine->bRecording && gpEngine->pRecordEnt == this)
		return;
	if(!IsHead)
		return;
	if(Pos.x<(gpEngine->Scroll-100))
	{
		vector<SolarFlare>::iterator iter;
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
bool SolarFlare::CheckCollided(Sphere s, float damage)
{
	if(!IsActive)
		return false;


	vector<SolarFlare>::iterator iter;
	for(iter = mSections.begin(); iter!=mSections.end(); iter++)
	{
		if(Collision::SphereSphereOverlap(s,(*iter).mSphere))
			return true;
	}

	if(!Collision::SphereSphereOverlap(s,mSphere))
		return false;
	return true;
}

void SolarFlare::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&bStartDirection,1);
	ReadBufferFromFile(fIO);
}
void SolarFlare::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&bStartDirection,1);
	WriteBufferToFile(fIO);
}
int SolarFlare::InWater()
{
	return 0;
}