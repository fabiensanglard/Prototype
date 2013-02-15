#pragma once
#include <vector>
#include <string>
using namespace std;
//================================================================================================//
						/******************
						** Sprite sheets **	
						*******************/
//================================================================================================//
class SpriteSheet
{
public:
	SpriteSheet();
	~SpriteSheet();
	void Split(unsigned int img, int imgWidth, int imgHeight, int width, int height);
	pair<float,float> GetFrameOffsets(int frame);
	pair<float,float> GetTextureScale();
	pair<int,int> GetWindowScale();
	pair<int,int> GetDimensions();
	int numFrames;
	unsigned int imgID;
private:
	
	vector<pair<float,float> > offsets;
	float texscale[2];
	int windowscale[2];
	pair<int,int> dimensions;

};