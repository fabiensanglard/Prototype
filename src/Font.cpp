#include "Font.h"
Font::Font()
{
	texID=0;
	srcBlend = GL_SRC_ALPHA;
	dstBlend = GL_ONE_MINUS_SRC_ALPHA;
	scale.x = 1;
	scale.y = 1;
	TextColor[0]=1;
	TextColor[1]=1;
	TextColor[2]=1;
	bShadow = true;
	mAlign = ALIGN_CENTER;
	sine =0;
	bSineWave = false;
}
Font::~Font()
{
	if(texID)
		glDeleteTextures(1,&texID);
}

bool Font::LoadFont(const char* name,const char* shadowname)
{
	if(!GenerateGlyphs(name))
		return false;
	GenerateCoords();

	if(!UTIL_GL::Image::LoadImage(shadowname))
		return false;
	UTIL_GL::TextureFilter(GL_LINEAR,GL_LINEAR);
	UTIL_GL::TextureWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	return true;
}
bool Font::GenerateGlyphs(const char* name)
{
	ILuint ImageName;
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	if(!ilLoadImage(name))
	{
		gLog.OutPut("Failed to load fontset: ");
		gLog.OutPut(name);
		gLog.OutPut("!\n");
		ilDeleteImages(1, &ImageName);
		return false;
	}
	gLog.OutPut("Image Loaded:");
	gLog.OutPut(name);
	gLog.OutPut(".\n");

	unsigned int width=ilGetInteger(IL_IMAGE_WIDTH), height=ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned char* data = ilGetData();

	GenerateOffsets(data,width,height);
	glGenTextures(1,&texID);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,texID);
	UTIL_GL::TextureFilter(GL_LINEAR,GL_LINEAR);
	UTIL_GL::TextureWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	ilDeleteImages(1, &ImageName);

	return true;
}
void Font::GenerateOffsets(unsigned char* data, int w, int h)
{
	int x,y,pixcount,ox,oy;
	int width = w/16;
	int height = h/16;
	for(ox=0;ox<w;ox+=width)
	{
		for(oy=0;oy<h;oy+=height)
		{
			pixcount=0;
			for(x=0;x<width;x++)
			{
				bool foundpixel = false;
				for(y=0;y<height;y++)
				{
					if(data[4*((ox+x)+((oy+y)*w))+3]>0)
					{
						foundpixel = true;
						break;
					}
					
				}
				if(foundpixel)
					pixcount++;
			}
			mGlyphs[(ox/width)+((oy/height)*16)].width = pixcount;
		}
	}
}
void Font::GenerateCoords()
{
	float u=0,v=0;
	int x,y;
	for(x=0;x<16;x++)
	{
		v=0;
		for(y=0;y<16;y++)
		{
			mGlyphs[x+(y*16)].tU = u;
			mGlyphs[x+(y*16)].tV = v;
			v+=(1.0f/16.0f);
		}
		u+=(1.0f/16.0f);
	}
}

void Font::SetBlendMode(unsigned int src, unsigned int dst)
{
	srcBlend = src;
	dstBlend = dst;
}
void Font::SetScale(float width, float height)
{
	scale.x = width;
	scale.y = height;
}
void Font::EnableShadow()
{
	bShadow = true;
}
void Font::DisableShadow()
{
	bShadow = false;
}

void Font::SetColor(float r, float g, float b, float a)
{
	TextColor[0]=r;
	TextColor[1]=g;
	TextColor[2]=b;
	TextColor[3]=a;
}
void Font::SetAlignment(int align)
{
	mAlign = align;
}
float Font::GetStringLength(const char* txt)
{
	float length=0;
	int l = (int)strlen(txt),n;
	for(n=0;n<l;n++)
	{
		if(txt[n]<=32)
			length+=(10.0f*scale.x);
		else
			length+=(mGlyphs[txt[n]].width*scale.x);
	}
	return length;
}
void Font::Print(const char* txt, float x, float y)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texID);
	glEnable(GL_BLEND);
	glBlendFunc(srcBlend,dstBlend);
	glColor4fv(TextColor);
	int LoopCount=1;
	switch(mAlign)
	{
	case ALIGN_CENTER:
		x-=(GetStringLength(txt)*0.5f);
		break;
	case ALIGN_RIGHT:
		x-=GetStringLength(txt);
		break;
	}
	float x2=x,y2=y;
	if(bShadow)
		LoopCount=2;
	int l = (int)strlen(txt),n,i;
	for(i=0;i<LoopCount;i++)
	{
		if(i==0 && bShadow)
		{
			glBindTexture(GL_TEXTURE_2D,texID+1);
			x+=(4.0f*scale.x);
			y+=(4.0f*scale.y);
		}
		else if(i || !bShadow)
		{
			glBindTexture(GL_TEXTURE_2D,texID);
			x=x2;
			y=y2;
		}
		float oldY= y;
		glBegin(GL_QUADS);
		for(n=0;n<l;n++)
		{
			if(bSineWave)
			{
				float sinus=sin(x*sine);				
				y=oldY+sin(sinus)*sineMulti;
			}
			if(txt[n]<=32 || mGlyphs[txt[n]].width<3)
			{
				x+=(10.0f*scale.x);
			}
			else
			{
				glTexCoord2f(mGlyphs[txt[n]].tU, mGlyphs[txt[n]].tV);
				glVertex2f(x,y);
				glTexCoord2f(mGlyphs[txt[n]].tU+(1.0f/16.0f), mGlyphs[txt[n]].tV);
				glVertex2f(x+(32*scale.x),y);
				glTexCoord2f(mGlyphs[txt[n]].tU+(1.0f/16.0f), mGlyphs[txt[n]].tV+(1.0f/16.0f));
				glVertex2f(x+(32*scale.x),y+(32*scale.y));
				glTexCoord2f(mGlyphs[txt[n]].tU, mGlyphs[txt[n]].tV+(1.0f/16.0f));
				glVertex2f(x,y+(32*scale.y));
				x+=(mGlyphs[txt[n]].width*scale.x);
			}
		}
		glEnd();
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
void Font::EnableSineWave(float value, float multi)
{
	bSineWave = true;
	sine = value;
	sineMulti = multi;
}
void Font::DisableSineWave()
{
	bSineWave = false;
}
