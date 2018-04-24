#pragma once

#include <vector>

#include <stb_image.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>


class Model
{
public:
	Model(std::string const &path, std::string textureFormat, bool gamma = false);
	void LoadTextures(int textureFlags);
	void SetupMesh();
	void Draw();

	enum {
		ALBEDO = 1 << 0,
		NORMAL = 1 << 1,
		ROUGHNESS = 1 << 2,
		METALLIC = 1 << 3,
		DEPTH = 1 << 4,
		AMBIENTOCCLUSION = 1 << 5
	};

private:
	void LoadModel(std::string const &path);
	void ProcessNode(aiNode *node, const aiScene *scene);
	void ProcessMesh(aiMesh *mesh, const aiScene *scene);
	unsigned int TextureFromFile(const char *name, std::string format, bool gamma = false);

	

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
	std::vector<unsigned int> indices;

	unsigned int textureMaps[6] = { 0, 0, 0, 0, 0, 0 };

	/*unsigned int albedoMap;
	unsigned int normalMap;
	unsigned int roughnessMap;
	unsigned int metallicMap;
	unsigned int depthMap;
	unsigned int AoMap;*/
};