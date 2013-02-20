#pragma once
//FCS #include <FMOD/fmod.h>
#include <stdio.h>

#define FSOUND_ALL 0

class Sound
{
public:
	virtual bool Load(const char* sound, bool loop)=0;
	
	virtual void Stop(int channel)=0;
	virtual void Discard()=0;
	void Volume(int channel,int vol);
};


class Sample : public Sound
{
public:
	Sample(){pSample = 0;}
//	~Sample(){ Discard(); }
	bool Load(const char* sound, bool loop);
	
	void Play(int channel);
	void Stop(int channel);
	void Discard();
	
	
private:
	//FCS FSOUND_SAMPLE* pSample;
	void* pSample;
};




class Stream : public Sound
{
public:
	Stream(){pStream = 0;}
//	~Stream(){ Discard(); }
	bool Load(const char* sound, bool loop);
	
	void Play(int channel);
	void Stop(int channel);
	void Discard();
	void Seek(int ms, int channel);
private:
	//FCSFSOUND_STREAM* pStream;
	void* pStream;
};

void FSOUND_Init(int mixrate, int maxchannels, int flags);
void FSOUND_Close(void);