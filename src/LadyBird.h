#pragma once
#include "Entity.h"

class LadyBird : public Entity
{
public:
	LadyBird();
	void Spawn(Vec2 pos);
	LadyBird* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	float sinus;
	int RateofFire;
	float fLastFireTime;
	bool bStartMovement;//start upwards or dowards

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
};