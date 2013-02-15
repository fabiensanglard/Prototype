#include "TriggerSpeedUp.h"
#include "Engine.h"
//================================================================================================//
						/********************************************************
						** Doesnt really do anything, more like a dummy entity **	
						*********************************************************/
//================================================================================================//

TriggerSpeedUp::TriggerSpeedUp()
{
	iMaxTicks = 10;
}
void TriggerSpeedUp::Spawn(Vec2 pos)
{
	IsActive = true;
	iMapSpeedTicks=0;
}
TriggerSpeedUp* TriggerSpeedUp::Clone()const
{
	return new TriggerSpeedUp(*this);
}
void TriggerSpeedUp::Update()
{
	gpEngine->fMapSpeed+=0.005f;
	iMapSpeedTicks++;
	if(iMapSpeedTicks>=iMaxTicks)
		IsActive = false;
}
void TriggerSpeedUp::Draw(const float interp)
{

}
void TriggerSpeedUp::NeedsToBeRemoved()
{
}
bool TriggerSpeedUp::CheckCollided(Sphere s, float damage)
{
	return false;
}
void TriggerSpeedUp::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&iMaxTicks,1);
}
void TriggerSpeedUp::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&iMaxTicks,1);
}
int TriggerSpeedUp::InWater()
{
	return 0;
}