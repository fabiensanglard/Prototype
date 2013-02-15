#pragma once
#include "Entity.h"

class StarBurst : public Entity
{
public:
	StarBurst();
	void Spawn(Vec2 pos);
	StarBurst* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	float sinus;
	int RateofFire;
	float fLastFireTime;
	int iTimesFired;
	bool bStartMovement;//start upwards or dowards

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);};