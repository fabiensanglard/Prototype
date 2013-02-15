#include "Sprite.h"
#include "Vector.h"
#include "UTIL_Misc.h"
//================================================================================================//
						/******************
						** Render sprite **	
						*******************/
//================================================================================================//

void RenderSprite(SpriteSheet spr, int frame, float x, float y, float w, float h)
{
	float hpix,vpix,s;
	pair<int,int> ws = spr.GetDimensions();
	hpix = (1.0f/(float)ws.first)*0.5f;
	vpix = (1.0f/(float)ws.second)*0.5f;
	UTIL_GL::BindTexture(GL_TEXTURE_2D,spr.imgID);

	pair<float,float>st = spr.GetFrameOffsets(frame);
	pair<float,float>scale = spr.GetTextureScale();
	s = st.first;
	float t = st.second;
	float tw = scale.first;
	float th = scale.second;

	glBegin(GL_QUADS);
	glTexCoord2f(s+hpix,t+vpix);
	glVertex2f(x,y);
	glTexCoord2f(s+tw-hpix,t+vpix);
	glVertex2f(x+w,y);
	glTexCoord2f(s+tw-hpix,t+th-vpix);
	glVertex2f(x+w,y+h);
	glTexCoord2f(s+hpix,t+th-vpix);
	glVertex2f(x,y+h);
	glEnd();
}
//================================================================================================//
						/*******************
						** Rotated sprite **	
						********************/
//================================================================================================//
void RenderRotatedSprite(SpriteSheet spr, int frame, float x, float y, float w, float h, float rot)
{
	float hpix,vpix,s;
	pair<int,int> ws = spr.GetDimensions();
	hpix = (1.0f/(float)ws.first)*0.5f;
	vpix = (1.0f/(float)ws.second)*0.5f;
	UTIL_GL::BindTexture(GL_TEXTURE_2D,spr.imgID);
	pair<float,float>st = spr.GetFrameOffsets(frame);
	pair<float,float>scale = spr.GetTextureScale();
	s = st.first;
	float t = st.second;
	float tw = scale.first;
	float th = scale.second;
	Vec2 pos(x,y);
	Vec2 r,u;
	UTIL_Misc::MakeVectors(rot,u,r);
	glBegin(GL_QUADS);
		glTexCoord2f(s+hpix,t+vpix);
		glVertex2fv((pos - r * w - u * h).v);
		glTexCoord2f((s+tw)-hpix,t+vpix);
		glVertex2fv((pos + r * w - u * h).v);
		glTexCoord2f((s+tw)-hpix,(t+th)-vpix);
		glVertex2fv((pos + r * w + u * h).v);
		glTexCoord2f(s+hpix,(t+th)-vpix);
		glVertex2fv((pos - r * w + u * h).v);
	glEnd();
}