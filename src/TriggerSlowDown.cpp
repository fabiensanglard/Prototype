#include "TriggerSlowDown.h"
#include "Engine.h"
//================================================================================================//
						/********************************************************
						** Doesnt really do anything, more like a dummy entity **	
						*********************************************************/
//================================================================================================//

TriggerSlowDown::TriggerSlowDown()
{

	iMaxTicks = 10;
}
void TriggerSlowDown::Spawn(Vec2 pos)
{
	IsActive = true;
	iMapSpeedTicks=0;
}
TriggerSlowDown* TriggerSlowDown::Clone()const
{
	return new TriggerSlowDown(*this);
}
void TriggerSlowDown::Update()
{
	gpEngine->fMapSpeed-=0.005f;
	iMapSpeedTicks++;
	if(iMapSpeedTicks>=iMaxTicks)
		IsActive = false;
}
void TriggerSlowDown::Draw(const float interp)
{

}
void TriggerSlowDown::NeedsToBeRemoved()
{
}
bool TriggerSlowDown::CheckCollided(Sphere s, float damage)
{
	return false;
}
void TriggerSlowDown::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&iMaxTicks,1);
}
void TriggerSlowDown::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&iMaxTicks,1);
}
int TriggerSlowDown::InWater()
{
	return 0;
}