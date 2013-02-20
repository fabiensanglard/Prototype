#pragma once
#include "Vector.h"
#include "UTIL_openGL.h"
#include "Log.h"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

class Font
{
	typedef struct Glyph_t
	{
		float tU,tV;
		int width;
	}Glpyh_s;
public:
	/*************************************
	 ** Methods							**
	 *************************************/
	Font();
	~Font();
	bool LoadFont(const char* name, const char* shadowname);
	void SetBlendMode(unsigned int src, unsigned int dst);
	void SetScale(float width, float height);
	void EnableShadow();
	void DisableShadow();
	void EnableSineWave(float value, float multi);
	void DisableSineWave();
	void SetColor(float r, float g, float b, float a);
	void SetAlignment(int align);
	float GetStringLength(const char* txt);
	void Print(const char* txt, float x, float y);
private:
	/*************************************
	 ** Methods							**
	 *************************************/
	bool GenerateGlyphs(const char* name);
	void GenerateOffsets(unsigned char* data, int w, int h);//variable widths
	void GenerateCoords();//texture coordinate per glyph
	/*************************************
	 ** variable						**
	 *************************************/
	Glyph_t	mGlyphs[256];
	unsigned int texID;
	unsigned int srcBlend,dstBlend;
	Vec2 scale;
	bool bShadow;
	float TextColor[4];
	int mAlign;
	float sine,sineMulti;
	bool bSineWave;
};