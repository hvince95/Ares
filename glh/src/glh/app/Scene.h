#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <map>
#include <string>

#include "../graphics/Camera.h"
#include "../graphics/Shader.h"
#include "../graphics/Entity.h"

namespace glh {
	namespace App {

		class Scene
		{
		public:
			void Init();
		private:
			std::vector<Graphics::Shader*> m_Shaders;
			std::vector<Graphics::Entity*> m_Entities;
			std::vector<Graphics::Camera*> m_Cameras;
		};

		void LoadMeshes(
			Scene& scene,
			const std::string& filename,
			std::vector<uint32_t>* loadedMeshIDs);

		void AddInstance(
			Scene& scene,
			uint32_t meshID,
			uint32_t* newInstanceID);
	}
}