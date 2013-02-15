#pragma once
#include "Entity.h"
using namespace std;

struct demoSystem
{
	int iScrollOffset;//startpoint in the map
	list<keyBuffer>	mKeyBuffer;
};

void SaveDemo(demoSystem dem);
bool LoadDemo(demoSystem& dem, int demo);