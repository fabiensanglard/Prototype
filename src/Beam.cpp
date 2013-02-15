#include "Beam.h"
#include "UTIL_openGL.h"
#include "UTIL_Misc.h"
#include "Engine.h"

struct Curve_t
{
	Vec2 ControlPoint[3];
};
inline Vec2 CalculatePointOnCurve(Curve_t c, float distance)
{
	float a = 1;
	float b = 0;
	float step = distance;
	Vec2 p;
	a-=step;
	b=1-a;
	p.x = c.ControlPoint[0].x*a*a + c.ControlPoint[2].x*2*a*b + c.ControlPoint[1].x*b*b;
	p.y = c.ControlPoint[0].y*a*a + c.ControlPoint[2].y*2*a*b + c.ControlPoint[1].y*b*b;
	return p;
}
//================================================================================================//
						/*****************
						** Spawn a beam **	
						******************/
//================================================================================================//
void Beam::Spawn(vector<Vec2> p, int segs, int randomness, float color[4], float duration, float thickness)
{
	IsActive = true;
	fDuration = duration;
	fThickness = thickness;
	int n;
	for(n=0;n<4;n++)
	{
		fStartColor[n] = color[n];
		fColor[n] = color[n];
		fOldColor[n] = color[n];
	}
	//create the beam
	float halfRand= (float)randomness*0.5f;
	Vec2 p1,p2;
	int i,k;
	bool bWay = (rand()%2? false: true);
	for(i=0, k=1; i<(int)p.size()-1; i++, k++)
	{
		p1=p[i];
		p2=p[k];
		mLinks.push_back(p1);
		Vec2 norm = MakeNormalForPoints(p1,p2);
		Vec2 mid;
		bWay? 
			mid= Vec2(p1 + ((p2-p1)*0.5f) + norm * (float)(halfRand + rand()%randomness)):
			mid= Vec2(p1 + ((p2-p1)*0.5f) - norm * (float)(halfRand + rand()%randomness));
		bWay = !bWay;
		float f = 1.0f/(float)(segs+1);
		float s= f;
		Curve_t c;
		c.ControlPoint[0]=p1;
		c.ControlPoint[2]=mid;
		c.ControlPoint[1]=p2;

		for(n=0; n<segs; n++)
		{
			mLinks.push_back(CalculatePointOnCurve(c,s));
			s+=f;
		}
	}
}
//================================================================================================//
						/********************
						** Update the beam **	
						*********************/
//================================================================================================//
void Beam::Update()
{
	vector<Vec2>::iterator iter;
	for(iter = mLinks.begin(); iter<mLinks.end(); iter++)
		(*iter).x += SCROLL_SPEED;

	fOldColor[3] = fColor[3];
	fColor[3] -= fDuration;

	int n = (int)mLinks.size();
	if(fColor[3]< -(fDuration*(n+2)))
		IsActive = false;
}
//================================================================================================//
						/********************
						** Render the beam **	
						*********************/
//================================================================================================//
void Beam::Render(const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);

	float r,g,b,a;
	r=UTIL_Misc::Interpolate(fColor[0],fOldColor[0],interp);
	g=UTIL_Misc::Interpolate(fColor[1],fOldColor[1],interp);
	b=UTIL_Misc::Interpolate(fColor[2],fOldColor[2],interp);
	a=UTIL_Misc::Interpolate(fColor[3],fOldColor[3],interp);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->sprGlowBullets.imgID);
	int i;
	int num = (int)mLinks.size();
	float alpha = fStartColor[3]/(float)num;
	a-=(alpha*num)*0.5f;
	Vec2 p,p2,p3,n,n2;
	glColor4f(0,0,0,0);
	glBegin(GL_QUADS);
	for(i=0; i<(int)mLinks.size()-2; i++)
	{
		p = mLinks[i];
		p2 = mLinks[i+1];
		p3 = mLinks[i+2];
		n=MakeNormalForPoints(p,p2)*fThickness;
		n2=MakeNormalForPoints(p2,p3)*fThickness;

		if(i>0)
			glColor4f(r,g,b,a<0?0:a);
		a+=alpha;
		if(a>1)a=1;
		glTexCoord2f(0.75f,0);
		glVertex2fv((p-n).v);
		glTexCoord2f(0.75f,1);
		glVertex2fv((p+n).v);
		glColor4f(r,g,b,a<0?0:a);
		glTexCoord2f(0.75f,1);
		glVertex2fv((p2+n2).v);
		glTexCoord2f(0.75f,0);
		glVertex2fv((p2-n2).v);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}