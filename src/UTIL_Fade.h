#pragma once
class Fade
{
public:
	Fade();
	void SetSrcColor(float r, float g, float b, float a);
	void SetDstColor(float r, float g, float b, float a);
	void GetColor(float time, float* rgba);//returns the current color
	void StartFade(float time, float duration);
	bool GetState(float time);//updates
	void UpdateFade(float time);
	void RenderFade(float time);
	void StartFadeIn(float time, int type, float duration);
	void StartFadeOut(float time, int type, float duration);
private:
	float mDuration;
	float mStart;
	float srcColor[4];
	float dstColor[4];
	float mScale;
	int mState;//type of fade to use

	void RenderDiamondStrip(float scale);
	void RenderHorizontalBars(float scale);
	void RenderFlat();
};