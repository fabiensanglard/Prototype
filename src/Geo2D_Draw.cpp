#include "Geo2D_Draw.h"
#include "UTIL_openGL.h"
//================================================================================================//
						/**********************************************
						** functions that draw the geometrix objects **	
						**********************************************/
//================================================================================================//
namespace GeoDraw2D
{
	void DrawLine(Line l)
	{
		glBegin(GL_LINES);
		glVertex2fv(l.s.v);
		glVertex2fv(l.e.v);
		//normal
		glColor3f(1,0,0);
		glVertex2fv((l.s + l.r*(l.d*0.5f)).v);
		glColor3f(1,1,0);
		glVertex2fv((l.s + l.r*(l.d*0.5f) + l.n*5).v);
		glEnd();
		glBegin(GL_POINTS);
		glColor3f(0,1,0);
		glVertex2fv(l.s.v);
		glColor3f(0,0,1);
		glVertex2fv(l.e.v);
		glEnd();

	}
	void DrawLine(Vec2 s, Vec2 e)
	{
		glBegin(GL_LINES);
		glVertex2fv(s.v);
		glVertex2fv(e.v);
		glEnd();
/*		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0,1,0);
		glVertex2fv(s.v);
		glColor3f(0,0,1);
		glVertex2fv(e.v);
		glEnd();
*/	}
	void DrawBox(Box b)
	{
		glBegin(GL_LINE_LOOP);
		glVertex2f(b.mins.x, b.mins.y);
		glVertex2f(b.maxs.x, b.mins.y);
		glVertex2f(b.maxs.x, b.maxs.y);
		glVertex2f(b.mins.x, b.maxs.y);
		glEnd();
		glBegin(GL_POINTS);
		glVertex2fv(b.p.v);
		glEnd();
	}
	void DrawSphere(Sphere s, int numPoints)
	{
		float degree = (float)(360/numPoints);
		float d=0;
		glBegin(GL_LINE_LOOP);
		for(int n=0; n<numPoints; n++)
		{
			glVertex2f(s.p.x + sin(d*PI/180) * s.r, s.p.y + cos(d*PI/180) * s.r);
			d+=degree;
		}
		glEnd();
		glBegin(GL_POINTS);
		glVertex2fv(s.p.v);
		glEnd();
	}
	void DrawIntersection(Vec2 entry, Vec2 exit)
	{
		glBegin(GL_POINTS);
		glColor3f(1,1,0);
		glVertex2fv(entry.v);
		glColor3f(0,1,1);
		glVertex2fv(exit.v);
		glEnd();
	}

	void DrawCapsule(Capsule c, int numPoints)
	{
		Vec2 r1, r2, r3, r4;
		r1 = Vec2(c.l.s + c.l.n * c.r) - c.l.s;
		r2 = Vec2(c.l.s + Vec2(0,-c.r)) - c.l.s;
		r3 = Vec2(c.l.e + c.l.n * c.r) - c.l.e;
		r4 = Vec2(c.l.e + Vec2(0,-c.r)) - c.l.e;
		glBegin(GL_LINES);
		glVertex2fv((c.l.s + c.l.n * c.r).v);
		glVertex2fv((c.l.e + c.l.n * c.r).v);
		glVertex2fv((c.l.s - c.l.n * c.r).v);
		glVertex2fv((c.l.e - c.l.n * c.r).v);
		glEnd();

		float degree = (float)(360/(numPoints-2));
		float d=0;
		glBegin(GL_LINE_STRIP);
		for(int n=0; n<numPoints; n++)
		{
			glVertex2f(c.l.e.x + sin(d*PI/180) * c.r, c.l.e.y + cos(d*PI/180) * c.r);
			d+=degree;
		}
		glEnd();
		d=0;
		glBegin(GL_LINE_STRIP);
		for(int n=0; n<numPoints; n++)
		{
			glVertex2f(c.l.s.x + sin(d*PI/180) * c.r, c.l.s.y + cos(d*PI/180) * c.r);
			d-=degree;
		}
		glEnd();

	}
}