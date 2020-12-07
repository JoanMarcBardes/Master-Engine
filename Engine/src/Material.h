#pragma once
#include "Component.h"
#include <vector>
#include <string>

class Material : public Component
{
public:
	Material(std::vector<unsigned int> texturesList, std::vector<std::string> pathList);

	void AddTexturePath(unsigned int texture, std::string path);

	void Draw(const unsigned program);

	std::vector<unsigned int> GetTextures() { return textures; }
	std::vector<std::string> GetPaths() { return paths; }

private:
	std::vector<unsigned int> textures;
	std::vector<std::string> paths;
};

