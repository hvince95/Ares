#include "Framebuffer.h"

#include <glad\glad.h>

#include "../util/Log.h"

namespace glh {
	namespace Graphics {

		Framebuffer::Framebuffer(int width, int height) {

			_width = width;
			_height = height;

			float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
				// positions   // texCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				1.0f, -1.0f,  1.0f, 0.0f,
				1.0f,  1.0f,  1.0f, 1.0f
			};

			// set up VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			// set up shader
			screenQuadShader = Shader("Data/Shaders/screenQuad.vs", "Data/Shaders/screenQuad.fs");
			screenQuadShader.use();
			screenQuadShader.setInt("screenTexture", 0);

			// set up framebuffer
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			// create a color attachment texture


			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)



			// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			Util::Log::WriteTrace("Framebuffer set up successfully");
		}

		void Framebuffer::CheckStatus() {
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				Util::Log::WriteError("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
			}
			else {
				Util::Log::WriteInfo("FRAMEBUFFER:: Framebuffer is okay");
			}
		}


		void Framebuffer::AddColourBuffer() {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glGenTextures(1, &textureColorbuffer);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void Framebuffer::AddDepthStencBuffer() {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			/*glGenTextures(1, &depthStenc);
			glBindTexture(GL_TEXTURE_2D, depthStenc);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStenc, 0);*/

			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Framebuffer::DrawToScreen() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

			glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			screenQuadShader.use();
			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glEnable(GL_DEPTH_TEST);
		}

		void Framebuffer::Bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		}

		void Framebuffer::Clear() {
			glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Framebuffer::Unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}