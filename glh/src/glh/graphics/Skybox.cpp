#include "Skybox.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "../Util/Log.h"


#include <vector>

namespace glh {
	namespace Graphics {

		void Skybox::Draw(glm::mat3 view, glm::mat4 projection) {
			skyboxShader.use();
			skyboxShader.setMat4("view", glm::mat4(view));
			skyboxShader.setMat4("projection", projection);

			glDepthFunc(GL_LEQUAL);
			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
			glDepthFunc(GL_LESS);
			//glBindVertexArray(0); // no need to unbind it every time as whenever we modify a vertex array we should bind it anyway
		}

		// loads a cubemap texture from 6 individual texture faces
		// order:
		// +X (right)
		// -X (left)
		// +Y (top)
		// -Y (bottom)
		// +Z (front) 
		// -Z (back)
		void Skybox::loadCubemapTexture(std::string skyboxName) {
			std::vector<std::string> faces
			{
				"Data/Skyboxes/" + skyboxName + "/right.jpg",
				"Data/Skyboxes/" + skyboxName + "/left.jpg",
				"Data/Skyboxes/" + skyboxName + "/top.jpg",
				"Data/Skyboxes/" + skyboxName + "/bottom.jpg",
				"Data/Skyboxes/" + skyboxName + "/front.jpg",
				"Data/Skyboxes/" + skyboxName + "/back.jpg"
			};

			glGenTextures(1, &cubemapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

			int width, height, nrChannels;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					Util::Log::WriteError("Cubemap texture failed to load at path: " + faces[i]);
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		Skybox::Skybox(std::string skyboxName) {

			loadCubemapTexture(skyboxName);

			skyboxShader = Shader("Data/Shaders/skybox.vs", "Data/Shaders/skybox.fs");

			skyboxShader.use();
			skyboxShader.setInt("skybox", 0);

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0);
		}

	}
}