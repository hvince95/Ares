#pragma once

#include <vector>

#include <stb_image.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Shader.h"

#include <array>

namespace glh {
	namespace Graphics {
		class Model
		{
		public:
			Model(std::string const &path, std::string textureFormat, bool gamma = false);
			void SetPosition(float posX, float posY, float posZ);
			void SetRotation(float rotX, float rotY, float rotZ);
			void SetScale(float scaleX, float scaleY, float scaleZ);
			unsigned int getVAO();
			void BindTextures();
			void SetModelMatrix(glm::mat4 model);

			// setup
			void LoadTextures(int textureFlags);
			void Draw(Shader* shader);

			enum {
				ALBEDO = 1 << 0,
				NORMAL = 1 << 1,
				ROUGHNESS = 1 << 2,
				METALLIC = 1 << 3,
				DEPTH = 1 << 4,
				AMBIENTOCCLUSION = 1 << 5
			};
			unsigned int textureMaps[6] = { 0, 0, 0, 0, 0, 0 };
			std::vector<unsigned int> indices;

		private:
			// setup methods
			void LoadModel(std::string const &path);
			void ProcessNode(aiNode *node, const aiScene *scene);
			void ProcessMesh(aiMesh *mesh, const aiScene *scene);
			void SetupMesh();
			unsigned int TextureFromFile(const char *name, std::string format, bool gamma = false);

			// Model physical attributes
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
			glm::mat4 modelMatrix;

			// Model Data
			std::string directory;
			std::string texFormat;
			bool gammaCorrection;

			//  Mesh Data  
			unsigned int VAO, VBO, EBO;

			struct Vertex {
				// position
				glm::vec3 Position;
				// normal
				glm::vec3 Normal;
				// texCoords
				glm::vec2 TexCoords;
				// tangent
				glm::vec3 Tangent;
				// bitangent
				glm::vec3 Bitangent;
			};

			std::vector<Vertex> vertices;

		};
	}
}