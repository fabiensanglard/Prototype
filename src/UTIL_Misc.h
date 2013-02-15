#pragma once
#include "Vector.h"

namespace UTIL_Misc
{
	float TimeRamp(float src, float dst, float startTime, float length, float curTime);
	void ConvertSecondsToHrsMinsSecs(int secondsIn, int& hoursOut, int& minsOut, int& secsOut);
	float GetZangleFrom4x4(float* mat);
	float Clamp(float in, float mins, float maxs);
	float Wrap(float in, float maxs);
	Vec2 Interpolate(Vec2 cur, Vec2 old, float interp);
	float Interpolate(float cur, float old, float interp);
	void MakeVectors(float rotation, Vec2& up, Vec2& right);//take in rotation degrees and return up/right vectors about it
}