#pragma once

#include "../Graphics/Shader.h"

#include <vector>

namespace glh {
	namespace Graphics {

		class Framebuffer {
		public:
			Framebuffer(int width, int height);
			void Bind();
			void Unbind();
			void Clear();
			void DrawToScreen();

			void CheckStatus();

			void AddColourBuffer();
			void AddDepthStencBuffer();


		private:
			unsigned int _width;
			unsigned int _height;

			unsigned int fbo;
			unsigned int textureColorbuffer;
			unsigned int rbo;
			unsigned int depthStenc;

			unsigned int quadVAO, quadVBO;
			std::vector<unsigned int> colourBuffers;

			Shader screenQuadShader;
		};
	}
}