#include "Geo2D.h"

//================================================================================================//
						/*********
						** Line **	
						*********/
//================================================================================================//
Line::Line()
{
	d = 0;
}
Line::Line(Vec2 start, Vec2 end)
{
	s = start;
	e = end;
	r = Normalize(e-s);
	n = MakeNormalForPoints(s,e);
	d = Distance(s,e);
}
scalar Line::Angle(Line l)
{
	return (acos(Dot(Normalize(s-e),Normalize(e-l.e))));//radians
}
//================================================================================================//
						/********
						** Box **	
						********/
//================================================================================================//
Box::Box()
{
	w=h=0;
}
Box::Box(scalar width, scalar height, Vec2 pos)
{
	w=width*0.5f;
	h=height*0.5f;
	p=pos;
	mins = Vec2(p.x-w,p.y-h);
	maxs = Vec2(p.x+w,p.y+h);
}
void Box::Offset(Vec2 newpos)
{
	p=newpos;
	mins = Vec2(p.x-w,p.y-h);
	maxs = Vec2(p.x+w,p.y+h);
}
//================================================================================================//
						/***********
						** sphere **	
						***********/
//================================================================================================//
Sphere::Sphere()
{
	r = 0;
}
Sphere::Sphere(scalar radius, Vec2 pos)
{
	r = radius;
	p = pos;
}
//================================================================================================//
						/************
						** capsule **	
						************/
//================================================================================================//
Capsule::Capsule()
{
	r=0;
}
Capsule::Capsule(scalar radius, Vec2 s, Vec2 e)
{
	r = radius;
	l = Line(s,e);
}
Capsule::Capsule(scalar radius, Line line)
{
	r = radius;
	l = line;
}
//================================================================================================//
						/********************
						** Collision tests **	
						********************/
//================================================================================================//
namespace Collision
{
	bool PointBoxOverlap(Vec2 p, Box b)
	{
		if(p.x<b.mins.x || p.x>b.maxs.x)
			return false;
		if(p.y<b.mins.y || p.y>b.maxs.y)
			return false;
		return true;
	}
	bool BoxBoxOverlap(Box a, Box b)
	{
		if(a.maxs.x<b.mins.x)
			return false;
		if(a.maxs.y<b.mins.y)
			return false;
		if(a.mins.x>b.maxs.x)
			return false;
		if(a.mins.y>b.maxs.y)
			return false;
		return true;
	}
	bool BoxBoxOverlap(Box a, Box b, Vec2& outNormal)
	{
		if(a.maxs.x<b.mins.x)
			return false;
		if(a.maxs.y<b.mins.y)
			return false;
		if(a.mins.x>b.maxs.x)
			return false;
		if(a.mins.y>b.maxs.y)
			return false;

		if(b.mins.x>=a.mins.x)
			outNormal.x-=1;
		if(b.maxs.x<=a.maxs.x)
			outNormal.x+=1;
		if(b.mins.y>=a.mins.y)
			outNormal.y-=1;
		if(b.maxs.y<=a.maxs.y)
			outNormal.y+=1;

		return true;
	}
	bool PointSphereOverlap(Vec2 p, Sphere s)
	{
		if(Distance(p,s.p)<=s.r)
			return true;
		return false;
	}
	bool SphereSphereOverlap(Sphere a, Sphere b)
	{
		scalar d = Distance(a.p,b.p);
		if(d<=(a.r+b.r))
			return true;
		return false;
	}
	bool SphereBoxOverlap(Sphere s, Box b)
	{
		Vec2 p,r;
		r = Normalize(b.p-s.p);
		p = s.p + r * s.r;
		if(PointBoxOverlap(p,b))
			return true;
		return false;
	}
	Vec2 ClosestPointOnLine(Vec2 p, Line l)
	{
		Vec2 out;
		scalar U;
		U = (((p.x-l.s.x)*(l.e.x-l.s.x))+((p.y-l.s.y)*(l.e.y-l.s.y))) / (l.d*l.d);
		if(U<0.0f)U=0.0f;
		if(U>1.0f)U=1.0f;
		out.x = l.s.x + U * (l.e.x-l.s.x);
		out.y = l.s.y + U * (l.e.y-l.s.y);
		return out;
	}
	scalar PointDistanceToLine(Vec2 p, Line l)
	{
		Vec2 out = ClosestPointOnLine(p,l);
		return Distance(out,p);
	}
	bool CollisionPrivate::LineQuickReject(Line a, Line b)
	{
		scalar minsxa,maxsxa,minsya,maxsya;
		minsxa= (a.s.x<a.e.x? a.s.x: a.e.x);
		maxsxa= (a.s.x<a.e.x? a.e.x: a.s.x);
		minsya= (a.s.y<a.e.y? a.s.y: a.e.y);
		maxsya= (a.s.y<a.e.y? a.e.y: a.s.y);
		scalar minsxb,maxsxb,minsyb,maxsyb;
		minsxb= (b.s.x<b.e.x? b.s.x: b.e.x);
		maxsxb= (b.s.x<b.e.x? b.e.x: b.s.x);
		minsyb= (b.s.y<b.e.y? b.s.y: b.e.y);
		maxsyb= (b.s.y<b.e.y? b.e.y: b.s.y);

		if(maxsxa<minsxb || minsxa>maxsxb)
			return false;
		if(maxsya<minsyb || minsya>maxsyb)
			return false;
		return true;
	}
	scalar LineDistance(Line a, Line b)
	{
		Vec2 s1,e1,s2,e2;
		scalar Ta,Tb;
		s1=a.s;
		s2=b.s;
		e1=a.e;
		e2=b.e;
		scalar denom, numeratorA,numeratorB;
		denom = (((e2.y-s2.y)*(e1.x-s1.x))-((e2.x-s2.x)*(e1.y-s1.y)));
		if(denom==0)//parallel
		{
			//make a 3rd line using one of the line's normal
			Line perp(a.s,a.s+a.n);
			//now we must do an infinite line intersection between the perpendicular line and the second line
			Vec2 p = LineSegmentIntersection(perp,b);
			Vec2 p2 = ClosestPointOnLine(p,a);
			return Distance(p,p2);
		}
		numeratorA = (((e2.x-s2.x)*(s1.y-s2.y))-((e2.y-s2.y)*(s1.x-s2.x)));
		numeratorB = (((e1.x-s1.x)*(s1.y-s2.y))-((e1.y-s1.y)*(s1.x-s2.x)));
		Ta = numeratorA / denom;
		Tb = numeratorB / denom;


		if(Ta>=0 && Ta<=1 && Tb>=0 && Tb<=1)//lines are intersecting inside the segment 
		{
			return 0;//no need for further testing, we know they intersect within segments
		}

		if(Ta<0)
			Ta=0;
		else if(Ta>1)
			Ta=1;
		Vec2 p,p2;
		p.x = s1.x + Ta * (e1.x-s1.x);
		p.y = s1.y + Ta * (e1.y-s1.y);
		p2 = ClosestPointOnLine(p,b);
		p = ClosestPointOnLine(p2,a);
		return Distance(p,p2);
	}
	Vec2 LineSegmentIntersection(Line a, Line b)
	{
		Vec2 p;
		Vec2 s1,e1,s2,e2;
		scalar Ta,Tb;
		s1=a.s;
		s2=b.s;
		e1=a.e;
		e2=b.e;
		scalar denom, numeratorA,numeratorB;
		denom = (((e2.y-s2.y)*(e1.x-s1.x))-((e2.x-s2.x)*(e1.y-s1.y)));
		if(denom==0)//parallel
			return p;
		numeratorA = (((e2.x-s2.x)*(s1.y-s2.y))-((e2.y-s2.y)*(s1.x-s2.x)));
		numeratorB = (((e1.x-s1.x)*(s1.y-s2.y))-((e1.y-s1.y)*(s1.x-s2.x)));
		Ta = numeratorA / denom;
		Tb = numeratorB / denom;

		if(Tb<0)
			Tb=0;
		else if(Tb>1)
			Tb=1;
		p.x = s2.x + Tb * (e2.x-s2.x);
		p.y = s2.y + Tb * (e2.y-s2.y);

		return p;
	}
	bool LineLineOverlap(Line a, Line b)
	{
		if(!CollisionPrivate::LineQuickReject(a,b))
			return false;
		Vec2 s1,e1,s2,e2;
		scalar Ta,Tb;
		s1=a.s;
		s2=b.s;
		e1=a.e;
		e2=b.e;
		scalar denom, numeratorA,numeratorB;
		denom = (((e2.y-s2.y)*(e1.x-s1.x))-((e2.x-s2.x)*(e1.y-s1.y)));
		if(denom==0)//parallel 
			return false;
		numeratorA = (((e2.x-s2.x)*(s1.y-s2.y))-((e2.y-s2.y)*(s1.x-s2.x)));
		numeratorB = (((e1.x-s1.x)*(s1.y-s2.y))-((e1.y-s1.y)*(s1.x-s2.x)));
		Ta = numeratorA / denom;
		Tb = numeratorB / denom;
		if(denom==0)//parallel 
			return false;
		if((Ta>=0 && Ta<=1) && (Tb>=0 && Tb<=1))
			return true;

		return false;
	}
	bool LineLineOverlap(Line a, Line b, Vec2& out)
	{
		if(!CollisionPrivate::LineQuickReject(a,b))
			return false;
		Vec2 s1,e1,s2,e2;
		scalar Ta,Tb;
		s1=a.s;
		s2=b.s;
		e1=a.e;
		e2=b.e;
		scalar denom, numeratorA,numeratorB;
		denom = (((e2.y-s2.y)*(e1.x-s1.x))-((e2.x-s2.x)*(e1.y-s1.y)));
		if(denom==0)//parallel
			return false;
		numeratorA = (((e2.x-s2.x)*(s1.y-s2.y))-((e2.y-s2.y)*(s1.x-s2.x)));
		numeratorB = (((e1.x-s1.x)*(s1.y-s2.y))-((e1.y-s1.y)*(s1.x-s2.x)));
		Ta = numeratorA / denom;
		Tb = numeratorB / denom;

		if((Ta>=0 && Ta<=1) && (Tb>=0 && Tb<=1))
		{
			out.x = s1.x + Ta * (e1.x-s1.x);
			out.y = s1.y + Ta * (e1.y-s1.y);
			return true;
		}
		return false;
	}

	bool LineBoxOverlap(Line l, Box b)
	{
		//Trivial Reject: if any of these pass the line is definetly outside the box
		if(l.s.x<b.mins.x && l.e.x<b.mins.x)
			return false;
		if(l.s.x>b.maxs.x && l.e.x>b.maxs.x)
			return false;
		if(l.s.y<b.mins.y && l.e.y<b.mins.y)
			return false;
		if(l.s.y>b.maxs.y && l.e.y>b.maxs.y)
			return false;

		Vec2 p = ClosestPointOnLine(b.p,l);
		if(!PointBoxOverlap(p,b))
			return false;
		return true;
	}
	bool LineBoxOverlap(Line l, Box b, Vec2& entry)
	{
		//Trivial Reject: if any of these pass the line is definetly outside the box
		if(l.s.x<b.mins.x && l.e.x<b.mins.x)
			return false;
		if(l.s.x>b.maxs.x && l.e.x>b.maxs.x)
			return false;
		if(l.s.y<b.mins.y && l.e.y<b.mins.y)
			return false;
		if(l.s.y>b.maxs.y && l.e.y>b.maxs.y)
			return false;
		//Trivial Reject2: Completely inside the box
		entry = l.s;
		if(PointBoxOverlap(l.s,b))
			return true;

		//clipping
		if(l.s.x<b.mins.x)
		{
			Line clip(Vec2(b.mins.x,-1000),Vec2(b.mins.x,1000));
			LineLineOverlap(l,clip,l.s);
		}
		else if(l.s.x>b.maxs.x)
		{
			Line clip(Vec2(b.maxs.x,-1000),Vec2(b.maxs.x,1000));
			LineLineOverlap(l,clip,l.s);
		}
		if(l.s.y<b.mins.y)
		{
			Line clip(Vec2(-1000,b.mins.y),Vec2(1000,b.mins.y));
			LineLineOverlap(l,clip,l.s);
		}
		else if(l.s.y>b.maxs.y)
		{
			Line clip(Vec2(-1000,b.maxs.y),Vec2(1000,b.maxs.y));
			LineLineOverlap(l,clip,l.s);
		}
		entry = l.s;
		if(!PointBoxOverlap(l.s,b))
			return false;
		return true;
	}
	bool LineBoxOverlap(Line l, Box b, Vec2& entry, Vec2& exit)
	{
		//Trivial Reject: if any of these pass the line is definetly outside the box
		if(l.s.x<b.mins.x && l.e.x<b.mins.x)
			return false;
		if(l.s.x>b.maxs.x && l.e.x>b.maxs.x)
			return false;
		if(l.s.y<b.mins.y && l.e.y<b.mins.y)
			return false;
		if(l.s.y>b.maxs.y && l.e.y>b.maxs.y)
			return false;
		//Trivial Reject2: Completely inside the box
		entry = l.s;
		exit = l.e;
		if(PointBoxOverlap(l.s,b) && PointBoxOverlap(l.e,b))
			return true;

		//clipping
		if(l.s.x<b.mins.x)
		{
			Line clip(Vec2(b.mins.x,-1000),Vec2(b.mins.x,1000));
			LineLineOverlap(l,clip,l.s);
		}
		else if(l.s.x>b.maxs.x)
		{
			Line clip(Vec2(b.maxs.x,-1000),Vec2(b.maxs.x,1000));
			LineLineOverlap(l,clip,l.s);
		}
		if(l.s.y<b.mins.y)
		{
			Line clip(Vec2(-1000,b.mins.y),Vec2(1000,b.mins.y));
			LineLineOverlap(l,clip,l.s);
		}
		else if(l.s.y>b.maxs.y)
		{
			Line clip(Vec2(-1000,b.maxs.y),Vec2(1000,b.maxs.y));
			LineLineOverlap(l,clip,l.s);
		}
		entry = l.s;
		if(l.e.x<b.mins.x)
		{
			Line clip(Vec2(b.mins.x,-1000),Vec2(b.mins.x,1000));
			LineLineOverlap(l,clip,l.e);
		}
		else if(l.e.x>b.maxs.x)
		{
			Line clip(Vec2(b.maxs.x,-1000),Vec2(b.maxs.x,1000));
			LineLineOverlap(l,clip,l.e);
		}
		if(l.e.y<b.mins.y)
		{
			Line clip(Vec2(-1000,b.mins.y),Vec2(1000,b.mins.y));
			LineLineOverlap(l,clip,l.e);
		}
		else if(l.e.y>b.maxs.y)
		{
			Line clip(Vec2(-1000,b.maxs.y),Vec2(1000,b.maxs.y));
			LineLineOverlap(l,clip,l.e);
		}
		exit = l.e;
		if(!PointBoxOverlap(l.s,b))
			return false;
		return true;
	}
	bool LineSphereOverlap(Line l, Sphere s)
	{
		if(PointDistanceToLine(s.p,l)<=s.r)
			return true;
		return false;
	}
	bool LineSphereOverlap(Line l, Sphere s, Vec2& entry)
	{
		if(PointDistanceToLine(s.p,l)>s.r)
			return false;

		scalar A,B,C;
		A = ((l.e.x-l.s.x)*(l.e.x-l.s.x)) + ((l.e.y-l.s.y)*(l.e.y-l.s.y));
		B = 2*( (l.e.x-l.s.x)*(l.s.x-s.p.x) + (l.e.y-l.s.y)*(l.s.y-s.p.y) );
		C = (s.p.x*s.p.x) + (s.p.y*s.p.y) + (l.s.x*l.s.x) + (l.s.y*l.s.y) - 2*( s.p.x*l.s.x + s.p.y*l.s.y ) - (s.r*s.r);

		scalar result = B*B-4*A*C;
		scalar U;
		if(result<0)
			return false;
		else if(result==0)
			U = -B/(2*A);
		else if(result>0)
		{
			if(PointSphereOverlap(l.s,s))
				U = (-B + sqrt( (B*B) - 4*A*C )) / (2*A);
			else
				U = (-B - sqrt( (B*B) - 4*A*C )) / (2*A);
		}
		entry.x = l.s.x + U * (l.e.x-l.s.x);
		entry.y = l.s.y + U * (l.e.y-l.s.y);
		return true;
	}
	bool LineSphereOverlap(Line l, Sphere s, Vec2& entry, Vec2& exit)
	{
		if(PointDistanceToLine(s.p,l)>s.r)
			return false;

		scalar A,B,C;
		A = ((l.e.x-l.s.x)*(l.e.x-l.s.x)) + ((l.e.y-l.s.y)*(l.e.y-l.s.y));
		B = 2*( (l.e.x-l.s.x)*(l.s.x-s.p.x) + (l.e.y-l.s.y)*(l.s.y-s.p.y) );
		C = (s.p.x*s.p.x) + (s.p.y*s.p.y) + (l.s.x*l.s.x) + (l.s.y*l.s.y) - 2*( s.p.x*l.s.x + s.p.y*l.s.y ) - (s.r*s.r);

		scalar result = B*B-4*A*C;

		scalar Ua,Ub;
		if(result<0)
			return false;
		else if(result==0.0f)
		{
			if(PointSphereOverlap(l.s,s))
			{
				Ub = -B/(2*A);
				entry.x = l.s.x;
				entry.y = l.s.y;
				exit.x = l.s.x + Ub * (l.e.x-l.s.x);
				exit.y = l.s.y + Ub * (l.e.y-l.s.y);
			}
			else
			{
				Ua = -B/(2*A);
				entry.x = l.e.x;
				entry.y = l.e.y;
				exit.x = l.s.x + Ua * (l.e.x-l.s.x);
				exit.y = l.s.y + Ua * (l.e.y-l.s.y);
			}
		}
		else if(result>0)
		{
			Ua = (-B - sqrt( (B*B) - 4*A*C )) / (2*A);
			Ub = (-B + sqrt( (B*B) - 4*A*C )) / (2*A);
			if(PointSphereOverlap(l.s,s))
			{
				entry = l.s;
				exit.x = l.s.x + Ub * (l.e.x-l.s.x);
				exit.y = l.s.y + Ub * (l.e.y-l.s.y);
			}
			else if(PointSphereOverlap(l.e,s))
			{
				entry.x = l.s.x + Ua * (l.e.x-l.s.x);
				entry.y = l.s.y + Ua * (l.e.y-l.s.y);
				exit = l.e;
			}
			else
			{
				entry.x = l.s.x + Ua * (l.e.x-l.s.x);
				entry.y = l.s.y + Ua * (l.e.y-l.s.y);
				exit.x = l.s.x + Ub * (l.e.x-l.s.x);
				exit.y = l.s.y + Ub * (l.e.y-l.s.y);
			}
		}
		return true;
	}

	bool BoxCapsuleOverlap(Box b, Capsule c)
	{
		Vec2 p,v,r;
		p = ClosestPointOnLine(b.p,c.l);
		r = Normalize(b.p-p);
		v = p + r * c.r;
		if(PointBoxOverlap(v,b))
			return true;
		return false;
	}
	bool SphereCapsuleOverlap(Sphere s, Capsule c)
	{
		if(PointDistanceToLine(s.p,c.l)>(s.r+c.r))
			return false;
		return true;
	}
	bool LineCapsuleOverlap(Line l, Capsule c)
	{
		if(LineDistance(l,c.l)>c.r)
			return false;
		return true;
	}

	bool LineCapsuleOverlap(Line l, Capsule c, Vec2& entry)
	{
		Vec2 ray = l.e - l.s;
		Vec2 vel = c.l.e - c.l.s;//sphere velocity
		Vec2 dif = l.s - c.l.s;
		scalar raydot = Dot(ray,ray);
		scalar veldot = Dot(vel,vel);
		scalar raydotvel = Dot(ray,vel);
		scalar raydotdif = Dot(ray,dif);
		scalar A = raydot * -veldot + raydotvel * raydotvel;
		scalar B = raydot * 2.0f * Dot(vel,dif) - 2.0f * raydotvel * raydotdif;
		scalar C = raydot * (c.r * c.r - Dot(dif,dif)) + raydotdif * raydotdif;
		scalar t;
		if(getLowestRoot(A,B,C,1.0f,t))
		{
			scalar u = (raydotvel * t - raydotdif) / raydot;
			if(u>=0 && u<=1)
			{
				entry = l.s + u *(l.e-l.s);
					return true;
			}
		}
		return false;
	}
	bool FindRoot(scalar a, scalar b, scalar c, scalar& r)
	{
		float d = b * b - 4.0f * a * c;
		if (d < 0.0f)
			return false;
		float inv2a = 0.5f / a;
		if (d == 0.0f)
			r = -b * inv2a;
		else
		{
			float sqrtd = sqrtf(d);
			scalar r1,r2;
			r1 = (-b - sqrtd) * inv2a;
			r2 =(-b + sqrtd) * inv2a;
			r1>r2? r=r2: r=r1;
		}
		return true;
	}
	bool getLowestRoot(scalar a, scalar b, scalar c, scalar maxR, scalar& root)
	{
			// Check if a solution exists
			float determinant = b*b - 4.0f*a*c;
			// If determinant is negative it means no solutions.
			if (determinant < 0.0f) return false;
			// calculate the two roots: (if determinant == 0 then
			// x1==x2 but let’s disregard that slight optimization)
			float sqrtD = sqrt(determinant);
			float r1 = (-b - sqrtD) / (2*a);
			float r2 = (-b + sqrtD) / (2*a);
			// Sort so x1 <= x2
			if (r1 > r2) {
				float temp = r2;
				r2 = r1;
				r1 = temp;
			}
			// Get lowest root:
			if (r1 > 0 && r1 < maxR) {
				root = r1;
				return true;
			}
			// It is possible that we want x2 - this can happen
			// if x1 < 0
			if (r2 > 0 && r2 < maxR) {
				root = r2;
				return true;
			}
			// No (valid) solutions
			return false;
		}

		bool PointSideOfLine(Vec2 p, Line l)
		{
			Vec2 c = l.s + l.r * (l.d * 0.5f);
			Vec2 p2 = c - l.n;
			Vec2 cp1(0,Cross(l.e-l.s, p-l.s));
			Vec2 cp2(0,Cross(l.e-l.s, p2-l.s));
			scalar d = Dot(cp1,cp2);
			if(d>=0)
				return true;
			return false;
		}
}
