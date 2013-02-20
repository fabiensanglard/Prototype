#pragma once
#include "Vector.h"
#include "Button.h"
#include "UTIL_Bitop.h"
#include "Sprite.h"
#include "UTIL_Misc.h"
#include "Geo2d.h"
#include "Geo2D_Draw.h"
#include "FileIO.h"
#include <list>
using namespace std;

class Entity
{
public:
	bool IsActive;
	virtual void Spawn(Vec2 pos)=0;
	virtual void Update()=0;
	virtual void NeedsToBeRemoved()=0;
	virtual void PreUpdate();
	virtual void Draw(const float interp)=0;
	virtual bool CheckCollided(Sphere s, float damage)=0;
	virtual Entity* Clone()const =0;
	bool CollidedWithMap();
	virtual int InWater()=0;//0= not in water, 1 = in water, 2 = water surface
	Vec2 oPos,Pos, Vel;
	float frame;
	float fLife,fStartLife;
	int iTakeDamageTicks;//display the entity as taking damage for n ticks

	Sphere mSphere;//ok not all ents will use a sphere - but MOST will

	virtual void LoadFromFile(CFileIO &fIO)=0;
	virtual void WriteToFile(CFileIO &fIO)=0;
    
    virtual ~Entity();
};
//checkout ControlledEntity::RecordKeyState for better info
struct keyBuffer
{
	int tick;
	unsigned char KeyStates;//in or out state of the key // if set key is in
	unsigned char KeyStateChange;//if set, just changed
};
class ControlledEntity : public Entity
{
public:
	ControlledEntity();
	virtual ~ControlledEntity();
	void PreUpdate(); 
	void RegisterControl(Button *buttons);
	void SelfControl();
	void RecordKeyState();
	void StartRecording();
	void PlayBack();
	bool bPlayBack;
	Button* mButtons;
	bool ClientControlled;
	int miStartTick,miCurTick;
	list<keyBuffer>	mKeyBuffer;

	void WriteBufferToFile(CFileIO &fIO);
	void ReadBufferFromFile(CFileIO &fIO);
};

#define KEY_UP		mButtons[0]
#define KEY_DOWN	mButtons[1]
#define KEY_RIGHT	mButtons[2]
#define KEY_LEFT	mButtons[3]
#define KEY_ATTACK1	mButtons[4]
#define KEY_ATTACK2	mButtons[5]
