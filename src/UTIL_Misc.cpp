#include "UTIL_Misc.h"

namespace UTIL_Misc
{
//================================================================================================//
						/*****************************************************
						 ** outputs a value between src & dst based on time **
						 *****************************************************/
//================================================================================================//
	float TimeRamp(float src, float dst, float startTime, float length, float curTime)

	{
		float t = ((curTime-startTime)/length);
		float l;
		src>dst?
			l=src-((src-dst)*t):
		l=src+((dst-src)*t);
		//clamp it
		if(src>dst)
		{
			if(l<dst)
				return dst;
			else if(l>src)
				return src;
		}
		else if(src<dst)
		{
			if(l>dst)
				return dst;
			else if(l<src)
				return src;
		}
		return l;
	}
//================================================================================================//
						/**********************************************
						 ** converts seconds to hours mins & seconds **
						 **********************************************/
//================================================================================================//
	void ConvertSecondsToHrsMinsSecs(int secondsIn, int& hoursOut, int& minsOut, int& secsOut)
	{
		hoursOut = (int)(secondsIn/(60*60));
		minsOut = (int)(secondsIn/60)-(60*hoursOut);
		secsOut = secondsIn>59?(int)secondsIn%60:(int)secondsIn;
	}
//================================================================================================//
						/*******************************************************
						 ** get the z axis angle in degrees from a 4x4 matrix **
						 *******************************************************/
//================================================================================================//
	float GetZangleFrom4x4(float* mat)
	{
		float Y,Z,C,trX,trY;
		Y = asin( mat[2]);
		C = cos( Y );
		if ( fabs( C ) > 0.000 )
		{
			trX = mat[0] / C;
			trY = -mat[1] / C;
		}
		else
		{
			trX = mat[5];
			trY = mat[4];
		}

		Z = RAD2DEG(atan2( trY, trX ));
		Z = Wrap(Z,360);
		return Z;
	}
//================================================================================================//
						/********************************
						 ** clamp a value to a min/max **
						 ********************************/
//================================================================================================//
	float Clamp(float in,float mins, float maxs)
	{
		if(in<mins)
			in = mins;
		if(in>maxs)
			in = maxs;
		return in;
	}
//================================================================================================//
						/*******************************
						 ** wrap a value to a min/max **
						 *******************************/
//================================================================================================//
	float Wrap(float in, float maxs)
	{
		if(in>maxs)
			in-=maxs;
		if(in<0)
			in+=maxs;
		return in;
	}
//================================================================================================//
						/*****************************
						 ** interpolation functions **
						 *****************************/
//================================================================================================//
	Vec2 Interpolate(Vec2 cur, Vec2 old, float interp)
	{
		Vec2 out;
		out = old + (cur-old) * interp;
		return out;
	}
	float Interpolate(float cur, float old, float interp)
	{
		float out;
		out = old + (cur-old) * interp;
		return out;
	}
//================================================================================================//
						/***********************************************
						 ** take a degree and return up/right vectors **
						 ***********************************************/
//================================================================================================//
	void MakeVectors(float rotation, Vec2& up, Vec2& right)
	{
		scalar cosYaw = (scalar)cos(DEG2RAD(rotation));
		scalar sinYaw = (scalar)sin(DEG2RAD(rotation));
		up = Vec2(cosYaw,sinYaw);
		right = Vec2(scalar(cos(DEG2RAD(rotation + 90.0))),scalar(sin(DEG2RAD(rotation + 90.0))));
		up.Normalize();
		right.Normalize();		
	}

}