#pragma once
#include "Entity.h"

class Orb : public Entity
{
public:
	Orb* Clone()const;
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	void GiveTarget(Vec2* target);
	int InWater();

	Vec2 *pTarget;//player position
	Vec2 mLockOffset;//offset from player position
	Vec2 mChain[6],mOChain[6],mChainOffset[6];
	bool bLockState;//is in chain or lock mode
	float rot;
	float fPulsate;
	float fGlowRotation;

	int iWeaponType;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
};