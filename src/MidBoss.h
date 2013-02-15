#pragma once
#include "Entity.h"

class MidBoss : public Entity
{
public:
	MidBoss();
	void Spawn(Vec2 pos);
	MidBoss* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	bool bJustSpawned;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	Sphere mSphereGun;//so we can shoot off the main weapon
	float fGunLife;
	int iGunDamageTicks;
	Sphere mBounds[6];//these spheres are for the shell of the enemy, main sphere is for the core
	float CoreColor[3];
	int iAttackPattern;
	float fAttackPatternTicks;//how long to stay in each pattern

	Vec2 mFootPatterns1[4];
	Vec2 mFootPatterns2[4];
	Vec2 mFootPatterns3[4];
	Vec2 mFoot[4];
	Vec2 mFootJoint[4];
	Vec2 mFootControl[4];
	
	Sphere mFootSphere[4][10];
	Vec2 mFootMotionSpline[4][3];
	float iTicksAlongMotion;

	bool bBigGun;
	float footRateOfFire;
	float CoreRateOfFire;
	bool bIsDead;
	int midActionFire;//during the second attack pattern gonna fire a spray of bullets
};