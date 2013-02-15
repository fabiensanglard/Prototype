#pragma once
#include "Entity.h"

class Fighter : public ControlledEntity
{
public:
	Fighter();
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	Fighter* Clone()const;

	bool bStartDirection;//start from left or right
	bool bDirection;//which way its facing
	bool bTurning;
	bool bAnim;//which way the anim is going
	int RateofFire;
	float fLastFireTime;
	bool bCrashing;
	int iNextSmokeTick;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);};