#pragma once
#ifdef WIN32
#include <sdl.h>
#else
#include <SDL/SDL.h>
#endif
#include "UTIL_openGL.h"
#include "Log.h"

//================================================================================================//
									/*****************
									 ** SDL helpers **
									 *****************/
//================================================================================================//
namespace UTIL_SDL
{
	bool InitSDL(char* winName, int width, int height, int bpp, bool vsync, bool fscreen);
	bool InitJoystick();
//================================================================================================//
									/*********************
									 ** pbuffer related **
									 *********************/
//================================================================================================//

	/* Note from Fabien Sanglard:
      Prototyp was written around 2005 using a branch of SDL: 1.3. Unfortunately it never reached a release
	  state. The pBuffer have since been "dropped". I have decided to emulate the pBuffer with OpenGL FBO.
	*/
	#ifndef SDL_VERSION_1_3
		#define SDL_RenderTarget GLuint	
	#endif

	SDL_RenderTarget* Create_pBuffer(unsigned int width, unsigned int height);//must be power of 2
	void Unlock_pBuffer(SDL_RenderTarget* pTarget);
	void Lock_pBuffer(SDL_RenderTarget* pTarget);
	void Bind_pBuffer(SDL_RenderTarget pTarget);
}


