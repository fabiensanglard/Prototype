#include "Sound.h"
#include "SDL.h"
#include "SDL_mixer.h"

void Sound::Volume(int channel,int vol){
	Mix_Volume(1,vol);
}

bool Sample::Load(char* sound, bool loop){
	this->pSample = Mix_LoadWAV(sound);
	return true;
}

void Sample::Play(int channel){
	Mix_PlayChannel (channel, (Mix_Chunk*)this->pSample, 0);
}

void Sample::Stop(int channel){
	Mix_HaltChannel(channel);
}

void Sample::Discard(){
	Mix_FreeChunk((Mix_Chunk*)this->pSample);
}



bool Stream::Load(char* stream, bool loop)
{
	pStream = Mix_LoadMUS(stream);
	return true;
}

void Stream::Play(int channel){
	Mix_PlayMusic((Mix_Music*)pStream,-1);
}

void Stream::Stop(int channel)
{
	Mix_HaltMusic();
}
void Stream::Discard()
{
	if(pStream)
		Mix_FreeMusic((Mix_Music*)pStream);
    
	pStream = 0;
}
void Stream::Seek(int ms, int channel)
{
	Mix_SetMusicPosition(ms/1000);
}





void FSOUND_Init(int mixrate, int maxchannels, int flags)
{
	
	SDL_Init(SDL_INIT_AUDIO);

	int audio_buffers = 4096;

	//Mix_AllocateChannels(maxchannels);

	if(Mix_OpenAudio(mixrate, AUDIO_U8,1, audio_buffers)) {
		printf("Unable to open audio!\n");
		getchar();
	}

	
  
}
void FSOUND_Close(void){
	Mix_CloseAudio();
}