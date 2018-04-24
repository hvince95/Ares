#pragma once

#include "Model.h"
#include <glm/glm.hpp>
#include <vector>

class ModelInstanced {
public:
	ModelInstanced(std::string const &path, std::string textureFormat, bool gamma = false);
	//TODO possibly remove glm from here, use a custom class defined as part of glh
	void AddInstance(glm::vec3 positions, glm::vec3 rotations);
	void AddInstance(glm::vec3 positions);
	void Draw();

private:
	Model model;
	unsigned int numInstances = 0;
	//TODO: would arrays be better here?
	std::vector<glm::vec3> _positions;
	std::vector<glm::vec3> _rotations;
};