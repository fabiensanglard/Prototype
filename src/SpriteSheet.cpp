#include "SpriteSheet.h"
//================================================================================================//
						/******************
						** Sprite sheets **	
						*******************/
//================================================================================================//
SpriteSheet::SpriteSheet()
{
	texscale[0]=0;
	texscale[1]=0;
	windowscale[0]=0;
	windowscale[1]=0;
	numFrames=0;
	imgID = 0;
}
SpriteSheet::~SpriteSheet()
{

}
//================================================================================================//
					/********************************************************
					** Take an image and make rectangular sections from it **	
					*********************************************************/
//================================================================================================//
void SpriteSheet::Split(unsigned int img, int imgWidth, int imgHeight, int width, int height)
{
	dimensions.first = imgWidth;
	dimensions.second = imgHeight;
	imgID = img;
	if(imgWidth<=1)
		return;
	if(imgHeight<=1)
		return;
	if(width<=1)
		return;
	if(height<=1)
		return;

	texscale[0]=(float)width/imgWidth;
	texscale[1]=(float)height/imgHeight;
	windowscale[0]=width;
	windowscale[1]=height;

	int x,y,i,j;
	for(y=0,i=0; y<imgHeight; y+=height,i++)
	{
		for(x=0,j=0; x<imgWidth; x+=width,j++)
		{
			offsets.push_back(make_pair((float)x/imgWidth,(float)y/imgHeight));
			numFrames++;
		}
	}
}
//================================================================================================//
					/*****************************************
					** get the offsets for a specific frame **	
					******************************************/
//================================================================================================//
pair<float,float> SpriteSheet::GetFrameOffsets(int frame)
{
	if(frame>numFrames)
		frame = numFrames;
	return offsets[frame];
}
pair<float,float> SpriteSheet::GetTextureScale()
{
	pair<float,float> out;
	out.first = texscale[0];
	out.second = texscale[1];
	return out;
}
pair<int,int> SpriteSheet::GetWindowScale()
{
	pair<int,int> out;
	out.first = windowscale[0];
	out.second = windowscale[1];
	return out;
}
pair<int,int> SpriteSheet::GetDimensions()
{
	return dimensions;
}