#pragma once

#include <glm\glm.hpp>

#include <iostream>

#include "../Graphics/Shader.h"
namespace glh {
	namespace Graphics {

		class Skybox
		{
		public:
			Skybox(std::string skyboxName);

			void Draw(glm::mat3 view, glm::mat4 projection);

		private:
			unsigned int VAO, VBO, EBO;
			unsigned int cubemapTexture;

			void loadCubemapTexture(std::string skyboxName);

			Shader skyboxShader;

			float vertices[24] = {
				// positions, Upper, Lower, Back, Front, Left, Right
				-1.0f,  1.0f, -1.0f, // UBL 0
				 1.0f,  1.0f, -1.0f, // UBR 1
				-1.0f,  1.0f,  1.0f, // UFL 2
				 1.0f,  1.0f,  1.0f, // UFR 3
				-1.0f, -1.0f, -1.0f, // LBL 4
				 1.0f, -1.0f, -1.0f, // LBR 5
				-1.0f, -1.0f,  1.0f, // LFL 6
				 1.0f, -1.0f,  1.0f  // LFR 7
			};
			unsigned int indices[39] = {
				0, 1, 2,	// top faces
				1, 3, 2,
				6, 2, 3,	// front faces
				3, 7, 6,
				6, 5, 4,	// bottom faces
				5, 6, 7,
				4, 5, 1,	// back faces
				1, 0, 4,
				4, 0, 2,	// left faces
				2, 6, 4,
				3, 1, 5,	// right faces
				5, 7, 3
			};

		};
	}
}