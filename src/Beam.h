#pragma once
#include "Vector.h"
#include <vector>
using namespace std;

class Beam
{
public:
	bool IsActive;
	vector<Vec2> mLinks;
	float fColor[4], fOldColor[4],fStartColor[4];
	float fDuration;//how fast the beam fades out
	float fThickness;//how wide the beam shall be
	void Spawn(vector<Vec2> p, int segs, int randomness, float color[4], float duration, float thickness);
	void Update();
	void Render(const float interp);
};