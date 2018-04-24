
#include "ModelInstanced.h"

ModelInstanced::ModelInstanced(std::string const &path, std::string textureFormat, bool gamma)
	: model(path, textureFormat, gamma)
{

}

void ModelInstanced::Draw() {

}

void ModelInstanced::AddInstance(glm::vec3 positions, glm::vec3 rotations)
{
	numInstances++;
	_positions.push_back(positions);
	_rotations.push_back(rotations);
}

void ModelInstanced::AddInstance(glm::vec3 positions)
{
	AddInstance(positions, glm::vec3(0, 0, 0));
}
