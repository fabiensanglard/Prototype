#pragma once
#include "Vector.h"
#include <vector>
using namespace std;
//================================================================================================//
						/*********
						** Line **	
						*********/
//================================================================================================//
class Line
{
public:
	Vec2 s,e,n,r;
	scalar d;

	Line();
	Line(Vec2 start, Vec2 end);
	scalar Angle(Line l);
};
//================================================================================================//
						/********
						** Box **	
						********/
//================================================================================================//
class Box
{
public:
	scalar w,h;
	Vec2 p,mins,maxs;
	Box();
	Box(scalar width, scalar height, Vec2 pos);
	void Offset(Vec2 newpos);//offset the box
};
//================================================================================================//
						/***********
						** sphere **	
						***********/
//================================================================================================//
class Sphere
{
public:
	scalar r;
	Vec2 p;
	Sphere();
	Sphere(scalar radius, Vec2 pos);
};
//================================================================================================//
						/************
						** capsule **	
						************/
//================================================================================================//
class Capsule
{
public:
	scalar r;
	Line l;
	Capsule();
	Capsule(scalar radius, Vec2 s, Vec2 e);
	Capsule(scalar radius, Line line);
};
//================================================================================================//
						/********************
						** Collision tests **	
						********************/
//================================================================================================//
namespace Collision
{
	bool PointBoxOverlap(Vec2 p, Box b);
	bool BoxBoxOverlap(Box a, Box b);
	bool BoxBoxOverlap(Box a, Box b, Vec2& outNormal);//returns a direction vector to denote which sides of the box where intersect (box b sides)
	bool PointSphereOverlap(Vec2 p, Sphere s);
	bool SphereSphereOverlap(Sphere a, Sphere b);
	bool SphereBoxOverlap(Sphere s, Box b);
	bool PointSideOfLine(Vec2 p, Line l);//returns true if a point is on the extending side of the line normal
	Vec2 ClosestPointOnLine(Vec2 p, Line l);//returns point on the line
	scalar PointDistanceToLine(Vec2 p, Line l);//returns distance
	scalar LineDistance(Line a, Line b);
	Vec2 LineSegmentIntersection(Line a, Line b);//returns a point on line b, line a is infinite
	bool LineLineOverlap(Line a, Line b);
	bool LineLineOverlap(Line a, Line b, Vec2& out);//returns the intersection point of two overlapping lines
	class CollisionPrivate
	{
		static bool LineQuickReject(Line a, Line b);//quick rejection test before moving onto intersection test
		friend bool LineLineOverlap(Line a, Line b);
		friend bool LineLineOverlap(Line a, Line b, Vec2& out);
	};
	bool LineBoxOverlap(Line l, Box b);
	bool LineBoxOverlap(Line l, Box b, Vec2& entry);//returns the first intersection point
	bool LineBoxOverlap(Line l, Box b, Vec2& entry, Vec2& exit);//returns the first intersection point and the punch out point
	bool LineSphereOverlap(Line l, Sphere s);
	bool LineSphereOverlap(Line l, Sphere s, Vec2& entry);//returns the entry point
	bool LineSphereOverlap(Line l, Sphere s, Vec2& entry, Vec2& exit);//returns the entry point and exit point

	bool BoxCapsuleOverlap(Box b, Capsule c);
	bool SphereCapsuleOverlap(Sphere s, Capsule c);
	bool LineCapsuleOverlap(Line l, Capsule c);
	bool LineCapsuleOverlap(Line l, Capsule c, Vec2& entry);//find the first intersection of a moving sphere(capsule) and a line

	bool FindRoot(scalar a, scalar b, scalar c, scalar& r);
	bool getLowestRoot(scalar a, scalar b, scalar c, scalar maxR, scalar& root);
}
