#include "CheckPoint.h"
#include "Engine.h"
//================================================================================================//
						/********************************************************
						** Doesnt really do anything, more like a dummy entity **	
						*********************************************************/
//================================================================================================//

CheckPoint::CheckPoint()
{
}
void CheckPoint::Spawn(Vec2 pos)
{
	IsActive = false;
	gpEngine->mPlayer.iSpawnOffset = (int)gpEngine->Scroll;

}
CheckPoint* CheckPoint::Clone()const
{
	return new CheckPoint(*this);
}
void CheckPoint::Update()
{
}
void CheckPoint::Draw(const float interp)
{
}
void CheckPoint::NeedsToBeRemoved()
{
}
bool CheckPoint::CheckCollided(Sphere s, float damage)
{
	return false;
}
void CheckPoint::LoadFromFile(CFileIO &fIO)
{
}
void CheckPoint::WriteToFile(CFileIO &fIO)
{
}
int CheckPoint::InWater()
{
	return 0;
}