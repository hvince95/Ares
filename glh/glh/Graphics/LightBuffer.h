#pragma once

/*
made specifically for shadows
*/
class LightBuffer
{
public:
	LightBuffer();
	unsigned int GetShadowBufferWidth();
	unsigned int GetShadowBufferHeight();
	void Bind();

private:
	unsigned int depthMapFBO;
	unsigned int SHADOW_WIDTH = 1024;
	unsigned int SHADOW_HEIGHT = 1024;;
};