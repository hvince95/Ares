#pragma once

#include "../Graphics/Shader.h"

class Framebuffer {
public:
	Framebuffer(int width, int height);
	void Bind();
	void Unbind();
	void Clear();
	void DrawToScreen();


private:
	unsigned int fbo;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	unsigned int depthStenc;

	unsigned int quadVAO, quadVBO;

	Shader screenQuadShader;
};