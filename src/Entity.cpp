#include "Entity.h"
#include "Engine.h"
//================================================================================================//
					/*******************************************************
					** test the ents bounding sphere agains the map tiles **
					********************************************************/
//================================================================================================//
bool Entity::CollidedWithMap()
{
	int x,y,mx,my;
	x = (int)((float)((mSphere.p.x-mSphere.r)/32.0f));
	mx = (int)((float)((mSphere.p.x+mSphere.r)/32.0f));
	y = (int)((float)((mSphere.p.y-mSphere.r)/32.0f));
	my = (int)((float)((mSphere.p.y+mSphere.r)/32.0f));
	x--;y--;mx++;my++;
	if(x<0)x=0;
	if(y<0)y=0;
	Box box;
	for(x; x<mx; x++)
	{
		for(y=0; y<my; y++)
		{
			if(gpEngine->mTilesLayer1[x][y].ID>=0 && gpEngine->mTilesLayer1[x][y].ID<512)
			{
				box= Box(32,32,Vec2((float)(x*32)+16, (float)(y*32)+16));
				if(Collision::SphereBoxOverlap(mSphere,box))
					return true;
			}
		}
	}
	return false;
}
//================================================================================================//
								/**************
								** preupdate **
								***************/
//================================================================================================//
void Entity::PreUpdate()
{
	NeedsToBeRemoved();
	Update();
	mSphere.p += (Pos-oPos);
}
//================================================================================================//
								/**********************
								** Controlled entity **
								***********************/
//================================================================================================//

ControlledEntity::ControlledEntity()
{
	IsActive = false;
	miStartTick = miCurTick = 0;
	mButtons = NULL;
	ClientControlled = true;
	bPlayBack = false;
}
ControlledEntity::~ControlledEntity()
{
	if(!ClientControlled)
		if(mButtons)
			delete [] mButtons;
}
//================================================================================================//
								/***************************
								** Start entity recording **
								***************************/
//================================================================================================//
void ControlledEntity::StartRecording()
{
	ClientControlled = true;
	miStartTick = miCurTick = 0;
	mKeyBuffer.clear();
}
//================================================================================================//
								/*****************************************
								** register input with entity recording **
								*****************************************/
//================================================================================================//
void ControlledEntity::RegisterControl(Button *buttons)
{
	mButtons=buttons;
}

//================================================================================================//
								/************************************************
								** Entity must control itself using buffer now **
								************************************************/
//================================================================================================//
void ControlledEntity::SelfControl()
{
	miStartTick = miCurTick = 0;
	ClientControlled = false;
	mButtons = NULL;
	mButtons = new Button[6];
}
//================================================================================================//
		/**********************************************************************************
		** record current keystate - this should only be called when a keystate changes! **
		***********************************************************************************/
//================================================================================================//
void ControlledEntity::RecordKeyState()
{
	keyBuffer t;
	UTIL_ClearBits<unsigned char>(t.KeyStates);
	UTIL_ClearBits<unsigned char>(t.KeyStateChange);
	t.tick = miCurTick;
	for(int n=0;n<6;n++)
	{
		if(mButtons[n].state == JUST_PRESSED || mButtons[n].state == PRESSED)
			UTIL_SetBit<unsigned char>(t.KeyStates,1<<(n+1));
		else
			UTIL_ClearBit<unsigned char>(t.KeyStates,1<<(n+1));
		if(mButtons[n].state == JUST_PRESSED || mButtons[n].state == JUST_RELEASED)
			UTIL_SetBit<unsigned char>(t.KeyStateChange,1<<(n+1));
		else
			UTIL_ClearBit<unsigned char>(t.KeyStateChange,1<<(n+1));
	}
	mKeyBuffer.push_back(t);
}
//================================================================================================//
						/*******************************
						** Playback the entity buffer **
						********************************/
//================================================================================================//
void ControlledEntity::PlayBack()
{
	if(mKeyBuffer.empty())
		return;
	keyBuffer k=mKeyBuffer.front();
	if(miCurTick>=k.tick)
	{
		for(int n=0;n<6;n++)
		{
			if(UTIL_IsBitSet<unsigned char>(k.KeyStates,1<<(n+1)))
			{
				if(UTIL_IsBitSet<unsigned char>(k.KeyStateChange,1<<(n+1)))
					mButtons[n].state = JUST_PRESSED;
				else
					mButtons[n].state = PRESSED;
			}
			else
			{
				if(UTIL_IsBitSet<unsigned char>(k.KeyStateChange,1<<(n+1)))
					mButtons[n].state = JUST_RELEASED;
				else
					mButtons[n].state = RELEASED;
			}
		}
		mKeyBuffer.pop_front();
	}
}
//================================================================================================//
						/***************
						** pre update **
						****************/
//================================================================================================//
void ControlledEntity::PreUpdate()
{
	NeedsToBeRemoved();
	miCurTick++;
	Update();
	mSphere.p += (Pos-oPos);
} 
void ControlledEntity::WriteBufferToFile(CFileIO &fIO)
{
	int bufsize = (int)mKeyBuffer.size();
	fIO.WriteBinary(&bufsize,1);
	if(bufsize>0)
	{
		list<keyBuffer>::iterator i;
		for(i = mKeyBuffer.begin(); i != mKeyBuffer.end(); i++)
		{
			fIO.WriteBinary(&(*i).tick,1);
			fIO.WriteBinary(&(*i).KeyStateChange,1);
			fIO.WriteBinary(&(*i).KeyStates,1);
		}
	}
}
void ControlledEntity::ReadBufferFromFile(CFileIO &fIO)
{
	int bufsize,n;
	fIO.ReadBinary(&bufsize,1);
	if(bufsize>0)
	{
		for(n=0;n<bufsize;n++)
		{
			keyBuffer kb;
			fIO.ReadBinary(&kb.tick,1);
			fIO.ReadBinary(&kb.KeyStateChange,1);
			fIO.ReadBinary(&kb.KeyStates,1);
			mKeyBuffer.push_back(kb);
		}
	}
}