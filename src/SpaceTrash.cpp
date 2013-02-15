#include "SpaceTrash.h"
#include "Engine.h"

SpaceTrash::SpaceTrash()
{
	SpreadRadius = 1;
}
//================================================================================================//
						/*******************
						** SpaceTrash spawn **	
						********************/
//================================================================================================//
void SpaceTrash::Spawn(Vec2 pos)
{
	IsActive = true;
	pos.y=-32;
	pos.x+=(float)(rand()%(SpreadRadius*32));
	oPos = Pos = pos;
	fLife = fStartLife = 1;
	frame = (float)(rand()%8);
	fRotation = (float)(rand()%10)+1;
	fGravity = (float)(rand()%4)+4;
	fRot = (float)(rand()%360);
	fGravity/=10.0f;
	rand()%2? bRotation = true: bRotation = false;
	mSphere = Sphere(15,pos+Vec2(16,16));
	iTakeDamageTicks = 0;
	mWaterticks = 0;
}
//================================================================================================//
						/*******************
						** SpaceTrash Clone **	
						********************/
//================================================================================================//
SpaceTrash* SpaceTrash::Clone()const
{
	return new SpaceTrash(*this);
}
//================================================================================================//
						/********************
						** SpaceTrash update **	
						*********************/
//================================================================================================//
void SpaceTrash::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;

	InWater();

	bRotation?
		fRot += fRotation:
		fRot -= fRotation;

	Vel.y+=fGravity;
	Vel.y*=0.92f;
	Pos.y+=Vel.y;
}
//================================================================================================//
						/******************
						** SpaceTrash draw **	
						*******************/
//================================================================================================//
void SpaceTrash::Draw(const float interp)
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
	RenderRotatedSprite(gpEngine->sprTrash,(int)frame,p.x+16,p.y+16,16,16,fRot);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 10);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void SpaceTrash::NeedsToBeRemoved()
{
	if(Pos.y>550)
		IsActive = false;
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool SpaceTrash::CheckCollided(Sphere s, float damage)
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
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Pos+Vec2(24,24),60,0.75f, (float)(RotAngle),false);
		SND_SMALLEXPLODE;
		gpEngine->mPlayer.iScore+=10;
	}
	iTakeDamageTicks = 3;
	return true;
}

void SpaceTrash::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&SpreadRadius,1);
}
void SpaceTrash::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&SpreadRadius,1);
}
int SpaceTrash::InWater()
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