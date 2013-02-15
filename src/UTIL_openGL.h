#pragma once
//#include <gl/glew.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <gl/gl.h>
#include <gl/glu.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>
#include <string>
#include <sstream>
#include "Log.h"
//#define EXTENSION_SUPPORT_1_1	

namespace UTIL_GL
{
//================================================================================================//
									/****************
									 ** 2D helpers **
									 ****************/
//================================================================================================//
	namespace GL2D
	{
		void SetOrtho(float w, float h);
		void DrawQuad(float sx, float sy, float ex, float ey);
		void DrawTextureQuad(float sx, float sy, float ex, float ey);//draws texture quad with 0-1 texture coordinates
		void DrawWireBox(float sx, float sy, float ex, float ey);
	}
//================================================================================================//
									/************************
								     ** texture enviroment **
									 ************************/
//================================================================================================//
	void TextureWrap(const unsigned int s, const unsigned int t);
	void TextureFilter(const unsigned int min, const unsigned int mag);
//================================================================================================//
									/*******************
									 ** image loading **
									 *******************/
//================================================================================================//
	namespace Image
	{
		//note these functions ignore enviroment and filter settings (uses devil's settings)
		GLuint LoadImage(std::string name);
		GLuint LoadImage(std::string name, const int min, const int mag, const int wrapx, const int wrapy);
		GLuint LoadImage(std::string name, unsigned int& width, unsigned int& height);
		unsigned char* LoadImageData(std::string name, unsigned int& width, unsigned int& height);//returns pixeldata - you must free this yourself!
		class ImagePrivate
		{
			static ILuint DevilLoadImage(std::string name);
			friend GLuint LoadImage(std::string name);
			friend GLuint LoadImage(std::string name, const int min, const int mag, const int wrapx, const int wrapy);
			friend GLuint LoadImage(std::string name, unsigned int& width, unsigned int& height);
			friend unsigned char*  LoadImageData(std::string name, unsigned int& width, unsigned int& height, unsigned int& bpp);
		};
	}
//================================================================================================//
									/***************
									 ** shortcuts **
									 ***************/
//================================================================================================//
	void BindTexture(const unsigned int type, unsigned int id);
	void SetBlend(const unsigned int src, const unsigned int dst);
	void AlphaTest(const unsigned int func, float alpha);
	void DepthTest(const unsigned int func);
	void StencilTest(const unsigned int func, int ref, unsigned int mask);
#ifdef EXTENSION_SUPPORT_1_1 
	void BindMultiTexture(const unsigned int channel, unsigned int id);//GL_TEXTURE_2D only 
#endif 
}

extern unsigned int textureMem;