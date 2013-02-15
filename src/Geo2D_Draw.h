#pragma once
#include "Geo2D.h"
//================================================================================================//
						/**********************************************
						** functions that draw the geometrix objects **	
						**********************************************/
//================================================================================================//
namespace GeoDraw2D
{
	void DrawLine(Line l);
	void DrawLine(Vec2 s, Vec2 e);
	void DrawBox(Box b);
	void DrawSphere(Sphere s, int numPoints);
	void DrawIntersection(Vec2 entry, Vec2 exit);
	void DrawCapsule(Capsule c, int numPoints);
}