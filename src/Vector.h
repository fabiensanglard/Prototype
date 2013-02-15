#pragma once
#include <math.h>
#define PI		(3.14159265359f)
#define DEG2RAD(a)	(PI/180*(a))
#define RAD2DEG(a)	(180/PI*(a))

typedef float scalar;
//typedef double scalar;
class Vec2
{
public:
	union
	{
		struct
		{
			scalar x,y;
		};
		scalar v[2];
	};
	Vec2(scalar a = 0, scalar b = 0) : x(a), y(b) {}
	Vec2(const Vec2 &vec) : x(vec.x), y(vec.y) {}
	Vec2(float* flvec) : x(flvec[0]), y(flvec[1]) {}
	const Vec2 &operator=(const Vec2& vec)
	{
		this->x=vec.x;
		this->y=vec.y;
		return *this;
	}
	const bool operator==(Vec2 vec) const
	{
		if(x!=vec.x || y!=vec.y)
			return false;
		return true;
	}
	const bool operator<(Vec2 vec) const
	{
		if(x>=vec.x && y>=vec.y)
			return false;
		return true;
	}
	const bool operator<=(Vec2 vec) const
	{
		if(x>vec.x && y>vec.y)
			return false;
		return true;
	}
	const bool operator>(Vec2 vec) const
	{
		if(x<=vec.x && y<=vec.y)
			return false;
		return true;
	}
	const bool operator>=(Vec2 vec) const
	{
		if(x<vec.x && y<vec.y)
			return false;
		return true;
	}
	const bool operator!=(Vec2 vec) const
	{ return !(*this==vec); }
	const Vec2 operator+(const Vec2& vec) const
	{ return Vec2(x+vec.x, y+vec.y); }
	const Vec2 operator-(const Vec2& vec) const
	{ return Vec2(x-vec.x, y-vec.y); }
	const Vec2 &operator+=(const Vec2& vec)
	{
		x+=vec.x; y+=vec.y;
		return *this;
	}
	const Vec2 &operator-=(const Vec2& vec)
	{
		x-=vec.x; y-=vec.y;
		return *this;
	}
//	const Vec2 operator-() const
//	{ return Vec2(-x,-y); }
	inline const Vec2 operator*(const scalar s) const
	{
		return Vec2(x*s,y*s);
	}
	inline const Vec2 &operator*=(const scalar s)
	{
		x*=s;y*=s;
		return (*this);
	}
	friend inline const Vec2 operator*(const scalar& s, Vec2 vec)
	{
		return vec*s;
	}
	inline const Vec2 operator*(const Vec2& vec)const
	{ return Vec2(x*vec.x,y*vec.y); }
	inline const Vec2 operator/(const scalar s)const
	{ return Vec2(x/s,y/s); }
	inline Vec2 operator/=(const scalar s)
	{
		x/=s;y/=s;
		return (*this);
	}
	friend inline const Vec2 operator/(const scalar& s, Vec2& vec)
	{
		vec.x/=s;vec.y/=s;
		return vec;
	}
	inline const scalar operator%(Vec2& vec)const//dot product
	{ return x*vec.x+y*vec.y; }
	inline scalar Dot(Vec2& vec)const
	{ return (*this)%vec; }
	inline const scalar operator^(Vec2& vec)const//cross product
	{ return x*vec.y-y*vec.x; }
	inline scalar Cross(Vec2& vec)
	{ return (*this)^vec; }
	inline scalar Length()
	{ return scalar(sqrt(x*x+y*y)); }
	inline scalar Distance(const Vec2& vec)
	{ 
		Vec2 dir(x-vec.x,y-vec.y);
		return dir.Length();
	}
	inline const Vec2 Normalize()
	{ return *this /= Length(); }
	inline void MakeVectors(Vec2& forward, Vec2& right)
	{
		scalar cosYaw = (scalar)cos(DEG2RAD(x));
		scalar sinYaw = (scalar)sin(DEG2RAD(x));
		forward = Vec2(cosYaw,sinYaw);
		right = Vec2(scalar(cos(DEG2RAD(x + 90.0))),scalar(sin(DEG2RAD(x + 90.0))));
		forward.Normalize();
		right.Normalize();		
	}
	inline scalar Angle(Vec2 vec)
	{
		return (scalar)atan2(vec.y - y, vec.x - x) * 180 / PI;
	}
};
inline scalar Dot(Vec2 a, Vec2 b)
{
	return a%b;
}
inline scalar Cross(Vec2 a, Vec2 b)
{ return a^b; }
inline const Vec2 Normalize(Vec2 v)
{ return v / v.Length(); }
inline const scalar Distance(Vec2 a, Vec2 b)
{
	Vec2 dir(a-b);
	return dir.Length();
}
inline scalar Length(Vec2 v)
{
	return scalar(sqrt(v.x*v.x+v.y*v.y));
}

inline scalar Angle(Vec2 a, Vec2 b)
{ return (scalar)atan2(b.y - a.y, b.x - a.x) * 180 / PI; }
inline Vec2 MakeNormalForPoints(Vec2 s, Vec2 e)
{
	Vec2 d=s-e;
	Vec2 n = Vec2(-d.y,d.x);
	return Normalize(n);
}
inline Vec2 OrthoProj(Vec2 U, Vec2 V)
{
	return (Dot(U,V) / Dot(U,U)) * U;
}
inline Vec2 Reflect(Vec2 v, Vec2 n, scalar r)
{
	v-= (1.0f + r) * Dot(v,n) * n;
	return  v;
}
/************************************************************************************************
	Vec3
*************************************************************************************************/
/*
class Vec3
{
public:
	union
	{
		struct
		{
			scalar x,y,z;
		};
		scalar v[3];
	};

	Vec3(scalar a = 0, scalar b = 0, scalar c = 0) : x(a), y(b), z(c) {}
	Vec3(const Vec3 &vec) : x(vec.x), y(vec.y), z(vec.z) {}
	Vec3(float* flvec) : x(flvec[0]), y(flvec[1]), z(flvec[2]) {}
	scalar &operator[](const long idx)
	{
		return *((&x)+idx);
	}

	const Vec3 &operator=(const Vec3& vec)
	{
		this->x=vec.x;
		this->y=vec.y;
		this->z=vec.z;
		return *this;
	}
	const Vec3 &operator=(const float* flvec)
	{
		x = flvec[0];
		y = flvec[1];
		z = flvec[2];

		return *this;
	}
	const bool operator==(Vec3 vec) const
	{
		if(x!=vec.x || y!=vec.y || z!=vec.z)
			return false;
		return true;
	}
	const bool operator!=(Vec3 vec) const
	{ return !(*this==vec); }
	const Vec3 operator+(const Vec3& vec) const
	{ return Vec3(x+vec.x, y+vec.y, z+vec.z); }
	const Vec3 operator-(const Vec3& vec) const
	{ return Vec3(x-vec.x, y-vec.y, z-vec.z); }
	const Vec3 &operator+=(const Vec3& vec)
	{
		x+=vec.x; y+=vec.y; z+=vec.z;
		return *this;
	}
	const Vec3 &operator-=(const Vec3& vec)
	{
		x-=vec.x; y-=vec.y; z-=vec.z;
		return *this;
	}
	const Vec3 operator-() const
	{ return Vec3(-x,-y,-z); }
	inline const Vec3 operator*(const scalar s) const
	{ return Vec3(x*s,y*s,z*s); }
	inline const Vec3 &operator*=(const scalar s)
	{
		x*=s;y*=s;z*=s;
		return (*this);
	}
	friend inline const Vec3 operator*(const scalar& s, Vec3& vec)
	{
		vec.x*=s;vec.y*=s;vec.z*=s;
		return vec;
	}
	inline const Vec3 operator*(const Vec3& vec)const
	{ return Vec3(x*vec.x,y*vec.y,z*vec.z); }
	inline const Vec3 operator/(const scalar s)const
	{ return Vec3(x/s,y/s,z/s); }
	inline Vec3 operator/=(const scalar s)
	{
		x/=s;y/=s;z/=s;
		return (*this);
	}
	friend inline const Vec3 operator/(const scalar& s, Vec3& vec)
	{
		vec.x/=s;vec.y/=s;vec.z/=s;
		return vec;
	}
	const scalar operator%(Vec3& vec)const
	{ return x*vec.x+y*vec.y+z*vec.z; }
	scalar Dot(Vec3& vec)const
	{ return (*this)%vec; }
	const Vec3 Cross(const Vec3 &vec) const
	{ return Vec3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x); }
	const Vec3 operator^(const Vec3 &vec) const
	{ return Vec3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x); }
	inline scalar Length()
	{ return scalar(sqrt(x*x+y*y+z*z)); }
	inline const Vec3 Normalize()
	{ return *this /= Length(); }
	inline scalar Distance(const Vec3& vec)
	{ 
		Vec3 dir(vec-(*this));
		return dir.Length();
	}
	void MakeVectors(Vec3& forward, Vec3& right, Vec3& up)
	{
		scalar cosYaw = (scalar)cos(DEG2RAD(x));
		scalar sinYaw = (scalar)sin(DEG2RAD(x));
		scalar sinPitch = (scalar)tan(DEG2RAD(y));
		
		forward = Vec3(cosYaw,sinPitch,sinYaw);
		right = Vec3(scalar(cos(DEG2RAD(x + 90.0))),0,scalar(sin(DEG2RAD(x + 90.0))));
		up = Vec3(0,0,0)-forward.Cross(right);
		forward.Normalize();
		right.Normalize();
		up.Normalize();	}
	// return angle between two vectors
	scalar Angle(Vec3 vec)
	{
		return atan2(vec.z - z, vec.x - x) * 180 / PI;
	}
	void AngleBetweenVector(Vec3& angles) const
	{
		float	tmp, yaw, pitch;
		
		if (z == 0 && x == 0)
		{
			yaw = 0;
			if (z > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			yaw = (float)(atan2(z, x) * 180 / PI);
			if (yaw < 0)
				yaw += 360;

			tmp = (float)sqrt (x*x + z*z);
			pitch = (float)(atan2(y, tmp) * 180 / PI);
			if (pitch < 0)
				pitch += 360;
		}
		
		angles.x = pitch;
		angles.z = yaw;
		angles.y = 0;
	}
};
inline scalar Dot(Vec3 a, Vec3 b)
{ return a%b; }
inline Vec3 Cross(Vec3 a, Vec3 b)
{ return a^b; }
inline const Vec3 Normalize(Vec3 v)
{ return v / v.Length(); }
inline const scalar Distance(Vec3 a, Vec3 b)
{
	Vec3 dir(a-b);
	return dir.Length();
}

inline Vec3 MakeNormalForPoints(Vec3 s, Vec3 e)
{
	return Normalize(Cross(s,e));
}
*/
//which side of a line a point is on
inline bool LineTest(Vec2 v1, Vec2 v2, Vec2 v3, Vec2 p)
{
	Vec2 A(v2-v1);
	Vec2 B(p-v1);
	Vec2 cp1(0, A.x*B.y - A.y*B.x);
	Vec2 C(v3-v1);
	Vec2 cp2(0, A.x*C.y - A.y*C.x);
	if(cp1.Dot(cp2) >=0 )
		return true;
	return false;
}
inline bool TrianglePointTest(Vec2 v[3], Vec2 p)
{
	if(!LineTest(v[1],v[2],v[0],p))
		return false;
	if(!LineTest(v[0],v[2],v[1],p))
		return false;
	if(!LineTest(v[0],v[1],v[2],p))
		return false;
	return true;
}
inline bool TriangleBoxTest(Vec2 v[3], Vec2 p, float w, float h)
{
	Vec2 v1 = Vec2(p.x-w,p.y-h);
	Vec2 v2 = Vec2(p.x+w,p.y-h);
	Vec2 v3 = Vec2(p.x+w,p.y+h);
	Vec2 v4 = Vec2(p.x-w,p.y+h);
	
	for(int n=0; n<3; n++)
	{
		if(n==0)
		{
			if(LineTest(v[1],v[2],v[0],v1))
				continue;
			if(LineTest(v[1],v[2],v[0],v2))
				continue;
			if(LineTest(v[1],v[2],v[0],v3))
				continue;
			if(LineTest(v[1],v[2],v[0],v4))
				continue;
			return false;
		}
		else if(n==1)
		{
			if(LineTest(v[0],v[2],v[1],v1))
				continue;
			if(LineTest(v[0],v[2],v[1],v2))
				continue;
			if(LineTest(v[0],v[2],v[1],v3))
				continue;
			if(LineTest(v[0],v[2],v[1],v4))
				continue;
			return false;
		}
		else if(n==2)
		{
			if(LineTest(v[0],v[1],v[2],v1))
				continue;
			if(LineTest(v[0],v[1],v[2],v2))
				continue;
			if(LineTest(v[0],v[1],v[2],v3))
				continue;
			if(LineTest(v[0],v[1],v[2],v4))
				continue;
			return false;
		}
	}
	return true;
}
//*/